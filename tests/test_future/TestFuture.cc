#include <iostream>
#include "net/EventLoop.h"
#include "net/ThreadPool.h"
#include "net/EventLoopGroup.h"
#include "net/Application.h"
#include "future/Future.h"

using namespace ananas;

template <typename Type>
Type ThreadFunc()
{
    std::cout << "SetValue 10\n";
    Type v = 10;
    return v;
}

void ThreadFuncV()
{
    std::cout << "SetValue Void\n";
}

int main()
{
    EventLoopGroup group(1);
    auto& loop = *group.SelectLoop();

    ananas::ThreadPool tpool;

    Future<int> ft(tpool.Execute(ThreadFunc<int>));

    ft.Then(&loop, [](int v) {
        std::cout << "1.Then got int value " << v
                  << " and return float 1.0f." << std::endl;
        return 1.0f;
    })
    .Then([](float f) {
        std::cout << "2.Then got float value " << f
                  << " and return nothing." << std::endl;
    })
    .Then(&loop, [&tpool]() {
        std::cout << "3.Then got void and return another future\n";

        return tpool.Execute(ThreadFuncV);
    })
    .Then([]() {
        std::cout << "4. Then GOODBYE!\n";
        Application::Instance().Exit();
    });

    std::cout << "BEGIN LOOP" << std::endl;

    loop.ScheduleAfter<ananas::kForever>(std::chrono::seconds(1), []() {
            printf("every 1 second\n");
            });

    auto& app = Application::Instance();
    app.Run();
    tpool.JoinAll();
            
    return 0;
}

