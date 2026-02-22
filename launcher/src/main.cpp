#include"MainWindow.hpp"
#include"LauncherInfo.hpp"

#include<thread>

int CALLBACK wWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPWSTR pCmdLine, INT nCmdShow){
    const int FRAME_TIME = 1000 / LauncherInfo::FRAME_RATE;

    MainWindow wnd(hInst, pCmdLine, L"MFO1 Launcher", 800, 600);

    while(wnd.ProcessMsg()){
        auto frameStart = std::chrono::high_resolution_clock::now();

        //hi

        auto frameEnd = std::chrono::high_resolution_clock::now();
		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart).count();
		if (elapsedTime < FRAME_TIME) {
			std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_TIME - elapsedTime));
		}
    }

    return 0;
}