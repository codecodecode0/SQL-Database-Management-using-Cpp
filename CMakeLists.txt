cmake_minimum_required(VERSION 3.10)
project(SP23Database)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(SP23Database
        main.cpp
        AppController.cpp
        AppController.hpp
        BasicTypes.hpp
        Config.hpp
        Errors.hpp
        Helpers.hpp
        keywords.hpp
        Scanner.hpp
        ScriptRunner.hpp
        TestAutomatic.hpp
        TestManually.hpp
        TestSequencer.hpp
        Timer.hpp
        Tokenizer.cpp
        Tokenizer.hpp
        View.hpp CmdConfigs.hpp BlockIO.cpp CmdProcessor.cpp CmdProcessor.hpp View.cpp SqlProcessor.cpp SqlProcessor.hpp CmdStatement.cpp CmdStatement.hpp Database.cpp Database.hpp Schema.hpp Attribute.cpp Schema.cpp Row.hpp RecordProcessor.cpp RecordProcessor.hpp Faked.cpp Row.cpp Filters.cpp Filters.hpp ParseHelper.cpp Compare.hpp Storage.cpp LRUCache.hpp Index.cpp Index.hpp)

