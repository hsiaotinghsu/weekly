// customNodesCppExamplePlugin02.cpp : Defines the entry point for the application.
//

#include <iostream>
#include "rocksolid/customNodes/cpp/CustomNodes.h"

namespace cpp_example
{
    void initSome3rdParty() {}
    void deinitSome3rdParty() {}

    namespace cpp = rocksolid::ar::customNodes::cpp;

    // this function will be called when runtime will ask for new plugin instance
    // can be any name, you pass it to RS_DEFINE_CPP_PLUGIN
    void createInstance(cpp::NodesRegistry& registry)
    {
        // if you want to allocate more resources during init of the instance
        // you can do it right here and free them in onDestroy method
        initSome3rdParty();
        auto someInstanceData = std::make_shared<size_t>();
        registry.subscribeOnDestroy(
            [someInstanceData]()
            {
                std::cout << "onDestroy: " << *someInstanceData << std::endl;
                deinitSome3rdParty();
            });

        // also you can subscribe on nodes pipeline execution events
        registry.subscribeOnStart(
            []()
            {
                std::cout << "start" << std::endl;
            });
        registry.subscribeOnResume(
            []()
            {
                std::cout << "resume" << std::endl;
            });
        registry.subscribeOnPause(
            []()
            {
                std::cout << "pause" << std::endl;
            });
        registry.subscribeOnStop(
            []()
            {
                std::cout << "stop" << std::endl;
            });

        // and finally you can register your nodes, primitive types supported by default
        registry.add("cpp_example",
            "calculateScale", // Node unique name
            {
                "Calculate scale factor out of velocity",
                "Custom Nodes", // Node category
                "Calculate Scale Factor", // Human readable name to show in ArStudio
            },
            [](double v)
            {
                auto constexpr factor = 0.25;
                std::cout << "Velocity: " << v << std::endl;
                auto scale = v * factor;
                return scale;
            },
            { "velocity" }, // you need to provide name for each argument of your function
            { "scale" } // and for each output. We have compile time check for this.
        );
    }
} // namespace cpp_example

/// you need to register you plugin, so it could be loadable by runtime
RS_DEFINE_CPP_PLUGIN(CustomNodePlugin, 1, 0, cpp_example::createInstance)

