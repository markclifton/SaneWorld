#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <time.h>
#include <cstdlib>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

std::string time_now()
{
    struct tm time_info;
    time_t time_create = std::time(NULL);
    localtime_s(&time_info, &time_create);
    char timebuf[26];
    asctime_s(timebuf, sizeof(timebuf), &time_info);
    std::string time(timebuf);
    time.pop_back();
    return time;
}

#define SANE_DEBUG(s) if(Sane::LogHandler::GetLogLevel() <= 0) std::cout << "[DEBUG] " << time_now() << " :: " << s << "\n";
#define SANE_INFO(s) if(Sane::LogHandler::GetLogLevel() <= 1) std::cout <<  "[INFO]  " << time_now() << " :: " << s << "\n";
#define SANE_WARN(s) if(Sane::LogHandler::GetLogLevel() <= 2) std::cout <<  "[WARN]  " << time_now() << " :: " << s << "\n";
#define SANE_FATAL(s) if(Sane::LogHandler::GetLogLevel() <= 3) std::cout << "[FATAL] " << time_now() << " :: " << s << "\n";

namespace Sane
{
    struct ImguiLogBox
    {
        ImGuiTextBuffer Buf;
        ImGuiTextFilter Filter;
        ImVector<int> LineOffsets;
        bool AutoScroll{ true };

        ImguiLogBox()
        {
            Clear();
        }

        void Clear()
        {
            Buf.clear();
            LineOffsets.clear();
            LineOffsets.push_back(0);
        }

        void AddLog(const char* fmt, ...) IM_FMTARGS(2)
        {
            int old_size = Buf.size();
            va_list args;
            va_start(args, fmt);
            Buf.appendfv(fmt, args);
            va_end(args);
            for (int new_size = Buf.size(); old_size < new_size; old_size++)
                if (Buf[old_size] == '\n')
                    LineOffsets.push_back(old_size + 1);
        }

        void Draw(const char* title, bool* p_open = NULL)
        {
            if (!ImGui::Begin(title, p_open))
            {
                ImGui::End();
                return;
            }

            // Options menu
            if (ImGui::BeginPopup("Options"))
            {
                ImGui::Checkbox("Auto-scroll", &AutoScroll);
                ImGui::EndPopup();
            }

            ImGui::SameLine();
            bool clear = ImGui::Button("Clear");
            ImGui::SameLine();
            bool copy = ImGui::Button("Copy");
            ImGui::SameLine();
            Filter.Draw("Filter", -100.0f);

            ImGui::Separator();
            ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

            if (clear)
                Clear();
            if (copy)
                ImGui::LogToClipboard();

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            const char* buf = Buf.begin();
            const char* buf_end = Buf.end();
            if (Filter.IsActive())
            {
                for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
                {
                    const char* line_start = buf + LineOffsets[line_no];
                    const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                    if (Filter.PassFilter(line_start, line_end))
                        ImGui::TextUnformatted(line_start, line_end);
                }
            }
            else
            {
                ImGuiListClipper clipper;
                clipper.Begin(LineOffsets.Size);
                while (clipper.Step())
                {
                    for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                    {
                        const char* line_start = buf + LineOffsets[line_no];
                        const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                        ImGui::TextUnformatted(line_start, line_end);
                    }
                }
                clipper.End();
            }
            ImGui::PopStyleVar();

            if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);

            ImGui::EndChild();
            ImGui::End();
        }
    };

    enum LOG_LEVEL {
        DEBUG = 0,
        INFO,
        WARN,
        FATAL
    };

    class LogHandler {
    private:
        LogHandler() = default;

        static LogHandler& Get() {
            static LogHandler logger;
            return logger;
        }

    public:
        ~LogHandler() = default;

        static void Create(LOG_LEVEL minLevel = LOG_LEVEL::DEBUG)
        {
            LogHandler& handler = Get();
            handler.minLevel = minLevel;
            handler.old_buffer = std::cout.rdbuf(handler.buffer.rdbuf());
        }

        static void Destroy()
        {
            LogHandler& handler = Get();
            std::cout.rdbuf(handler.old_buffer);
        }

        static void Display() {
            LogHandler& handler = Get();
            for (std::string line; std::getline(handler.buffer, line);) {
                handler.imgui.AddLog("%s\n", line.c_str());
            }
            handler.buffer.clear();

            handler.imgui.Draw("Log", &handler.visible);
        }

        static const LOG_LEVEL GetLogLevel() {
            return Get().minLevel;
        }

    private:
        bool visible{ true };
        LOG_LEVEL minLevel;
        std::stringstream buffer;
        std::streambuf* old_buffer;

        ImguiLogBox imgui;
    };
}