#pragma once
#include <string>

class Create_Html {
public:
    Create_Html() = default;
    ~Create_Html() = default;
    std::string load_html_string(std::vector<std::string> video_file_names) {
        std::string html = html_before_video_list;
        for (const auto& name : video_file_names) {
            html += "<option value='" + name + "'>" + name + "</option>\n";
        }
        html += html_after_video_list;
        return html;
    };
private:
    std::string html_before_video_list =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Connection: close\r\n\r\n"
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head><title>Watch Video</title></head>\n"
        "<body>\n"
        "<h1>Watch Video</h1>\n"
        "<button onclick=\"window.location.href='/'\">Go to Home</button>\n"
        "<br><br>\n"
        "<label for='videoSelect'>Choose a video:</label>\n"
        "<select id='videoSelect' onchange='changeVideo()'>\n";
    std::string html_after_video_list =
        "  </select>\n"
        "  <button id='loadBtn'>Load Video</button>\n"
        "  <br><br>\n"
        "  <div id='playerContainer' style='display:none;'>\n"
        "    <video id='videoPlayer' width='720' controls>\n"
        "      <source id='videoSource' type='video/mp4'>\n"
        "      Your browser does not support the video tag.\n"
        "    </video>\n"
        "  </div>\n"
        "  <script>\n"
        "    const select = document.getElementById('videoSelect');\n"
        "    const loadBtn = document.getElementById('loadBtn');\n"
        "    const playerDiv = document.getElementById('playerContainer');\n"
        "    const videoPlayer = document.getElementById('videoPlayer');\n"
        "    const videoSource = document.getElementById('videoSource');\n"
        "    loadBtn.addEventListener('click', () => {\n"
        "      // Only now do we set the src and show the player\n"
        "      videoSource.src = '/video?name=' + select.value;\n"
        "      videoPlayer.load();\n"
        "      playerDiv.style.display = 'block';\n"
        "      // Optionally auto‑play:\n"
        "      // videoPlayer.play();\n"
        "    });\n"
        "  </script>\n"
        "</body>\n"
        "</html>\n";
};