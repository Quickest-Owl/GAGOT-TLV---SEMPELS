///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2017, STEREOLABS.
//
// All rights reserved.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

#pragma once

static bool exit_app = false;

// Handle the CTRL-C keyboard signal
#ifdef _WIN32
void CtrlHandler(DWORD fdwCtrlType) {
    exit_app = (fdwCtrlType == CTRL_C_EVENT);
}
#else
#include <signal.h>
void nix_exit_handler(int s) {
    exit_app = true;
}
#endif

// Set the function to handle the CTRL-C
void SetCtrlHandler() {
#ifdef _WIN32
    SetConsoleCtrlHandler((PHANDLER_ROUTINE) CtrlHandler, TRUE);
#else // unix
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = nix_exit_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
#endif
}

// Display progress bar
void ProgressBar(float ratio, unsigned int w) {
    int c = int(ratio * w);
    for (int x = 0; x < c; x++) std::cout << "=";
    for (int x = c; x < int(w); x++) std::cout << " ";
    std::cout << (int) (ratio * 100) << "% ";
    std::cout << "\r" << std::flush;
}

// If the current project uses openCV
#if defined (__OPENCV_ALL_HPP__) || defined(OPENCV_ALL_HPP)
// Conversion function between sl::Mat and cv::Mat
cv::Mat slMat2cvMat(sl::Mat &input) {
    int cv_type = -1;
    switch (input.getDataType()) {
        case sl::MAT_TYPE_32F_C1: cv_type = CV_32FC1; break;
        case sl::MAT_TYPE_32F_C2: cv_type = CV_32FC2; break;
        case sl::MAT_TYPE_32F_C3: cv_type = CV_32FC3; break;
        case sl::MAT_TYPE_32F_C4: cv_type = CV_32FC4; break;
        case sl::MAT_TYPE_8U_C1: cv_type = CV_8UC1; break;
        case sl::MAT_TYPE_8U_C2: cv_type = CV_8UC2; break;
        case sl::MAT_TYPE_8U_C3: cv_type = CV_8UC3; break;
        case sl::MAT_TYPE_8U_C4: cv_type = CV_8UC4; break;
        default: break;
    }
    // Since cv::Mat data requires a uchar* pointer, we get the uchar1 pointer from sl::Mat (getPtr<T>())
    // cv::Mat and sl::Mat will share a single memory structure
    return cv::Mat(int(input.getHeight()), int(input.getWidth()), cv_type, input.getPtr<sl::uchar1>(sl::MEM_CPU));
}
#endif