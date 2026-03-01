/*
 * Based on the example server given in lsp-framework repository
 */

#include <charconv>
#include <chrono>
#include <iostream>
#include <lsp/connection.h>
#include <lsp/io/standardio.h>
#include <lsp/messagehandler.h>
#include <lsp/messages.h>

#include "assembler/Assembler.h"
#include "assembler/Linter.h"

namespace
{
    template <typename MessageType>
    void printMessageMethod()
    {
        const auto type = lsp::message::IsNotification<MessageType> ? "notification" : "request";
        std::cerr << "Server received " << type << " '" << MessageType::Method << '\'' << std::endl;
    }

    template <typename MessageType>
    void printMessagePayload(const typename MessageType::Params& params)
    {
        const auto json = lsp::toJson(typename MessageType::Params(params));
        std::cerr << "payload: " << lsp::json::stringify(json, true) << std::endl;
    }

    template <typename MessageType>
    void printMessage(const typename MessageType::Params& params)
    {
        printMessageMethod<MessageType>();
        printMessagePayload<MessageType>(params);
    }

    template <typename MessageType>
    void printMessage()
    {
        printMessageMethod<MessageType>();
    }

    void publishDiagnostics(
        lsp::MessageHandler &handler,
        lsp::DocumentUri uri,
        const std::string &source)
    {
        assembler::Linter linter(false);
        assembler::Assembler::assemble_from_text(source, linter, false);

        std::vector<lsp::Diagnostic> diagnostics;

        for (const auto& err : linter.get_errors())
        {
            const uint32_t line = err.line_number > 0 ? err.line_number - 1 : 0;
            const uint32_t col  = err.token_index + err.original_parsed_line.column_offset;

            lsp::Diagnostic diag;
            diag.range = lsp::Range{
                .start = lsp::Position{ .line = line, .character = col },
                .end   = lsp::Position{ .line = line, .character = col + err.original_line.size() }
            };

            diag.severity = lsp::DiagnosticSeverity::Error;
            diag.message  = std::string(err.message);
            diag.code     = std::string("1");
            diag.source   = std::string("StegasmLspServer");
            diag.relatedInformation = {};

            diagnostics.push_back(std::move(diag));
        }

        handler.sendNotification<lsp::notifications::TextDocument_PublishDiagnostics>(
            lsp::notifications::TextDocument_PublishDiagnostics::Params{
                .uri = uri,
                .diagnostics = std::move(diagnostics)
            }
        );
    }

    /*
     * Callback registration
     */

    thread_local bool g_running = false;

    void registerCallbacks(lsp::MessageHandler& messageHandler)
    {
        messageHandler.add<lsp::requests::Initialize>(
            [](lsp::requests::Initialize::Params&& params)
            {
                return lsp::requests::Initialize::Result{
                    .capabilities = {
                        .positionEncoding = lsp::PositionEncodingKind::UTF16,
                        .textDocumentSync = lsp::TextDocumentSyncOptions{
                            .openClose = true,
                            .change = lsp::TextDocumentSyncKind::Full,
                            .save = true
                        },
                        .hoverProvider = true
                    },
                    .serverInfo = lsp::InitializeResultServerInfo{
                        .name = "StegasmLspServer",
                        .version = "1.0.0"
                    },
                };
            }
        ).add<lsp::notifications::TextDocument_DidOpen>(
            [&messageHandler](lsp::notifications::TextDocument_DidOpen::Params&& params)
            {
                publishDiagnostics(messageHandler,
                                   params.textDocument.uri,
                                   params.textDocument.text);
            }
        ).add<lsp::notifications::TextDocument_DidChange>(
            [&messageHandler](lsp::notifications::TextDocument_DidChange::Params&& params)
            {
                if (!params.contentChanges.empty())
                {
                    const auto& change = params.contentChanges.front();

                    if (const auto* full = std::get_if<lsp::TextDocumentContentChangeEvent_Text>(&change))
                        publishDiagnostics(messageHandler, params.textDocument.uri, full->text);
                }
            }
        ).add<lsp::notifications::TextDocument_DidSave>(
            [&messageHandler](lsp::notifications::TextDocument_DidSave::Params&& params)
            {
                if (params.text.has_value())
                    publishDiagnostics(messageHandler, params.textDocument.uri, *params.text);
            }

        ).add<lsp::requests::TextDocument_Hover>(
            [](lsp::requests::TextDocument_Hover::Params&& params)
            {
                return std::async(std::launch::deferred,
                                  [params = std::move(params)]()
                                  {
                                      auto hover = lsp::Hover{
                                          .contents = ""
                                      };
                                      return lsp::requests::TextDocument_Hover::Result(std::move(hover));
                                  }
                );
            }
        ).add<lsp::requests::Shutdown>(
            []()
            {
                return lsp::requests::Shutdown::Result();
            }
        ).add<lsp::notifications::Exit>(
            []()
            {
                g_running = false;
            }
        );
    }

    /*
     * Message processing loop
     */

    void runLanguageServer(lsp::io::Stream& io)
    {
        try
        {
            auto connection = lsp::Connection(io);
            auto messageHandler = lsp::MessageHandler(connection);
            registerCallbacks(messageHandler);

            g_running = true;

            while (g_running)
                messageHandler.processIncomingMessages();
        }
        catch (const std::exception& e)
        {
            std::cerr << "ERROR: " << e.what() << std::endl;
        }
    }

    /*
     * stdio server
     */

    void runStdioServer()
    {
        runLanguageServer(lsp::io::standardIO());
    }
} // namespace

int main(int argc, char** argv)
{
    try
    {
        runStdioServer();
        std::cerr << "Exiting" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
