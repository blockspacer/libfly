SRC_DIRS_$(d) := \
    fly/coders \
    fly/coders/base64 \
    fly/coders/huffman \
    fly/logger \
    fly/parser \
    fly/system \
    fly/task \
    fly/types/bit_stream \
    fly/types/bit_stream/detail \
    fly/types/json \
    fly/types/string \
    bench/util \
    test/util

# Include the directories containing the benchmark tests.
SRC_DIRS_$(d) += \
    bench/coders \
    bench/json

SRC_$(d) := \
    $(d)/main.cpp

CXXFLAGS_$(d) += -I$(SOURCE_ROOT)/test/Catch2/single_include
CXXFLAGS_$(d) += \
    -DCATCH_CONFIG_PREFIX_ALL \
    -DCATCH_CONFIG_FAST_COMPILE \
    -DCATCH_CONFIG_ENABLE_OPTIONAL_STRINGMAKER
