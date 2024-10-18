#include <common/common.hpp>
#include <catch_amalgamated.hpp>

//
// Tests.
//

TEST_CASE("float3 - abs", "[float3]") {
    using namespace fb;
    REQUIRE(float3_abs(float3(0.0f, 0.0f, 0.0f)) == float3(0.0f, 0.0f, 0.0f));
    REQUIRE(float3_abs(float3(-1.0f, 0.0f, 0.0f)) == float3(1.0f, 0.0f, 0.0f));
    REQUIRE(float3_abs(float3(0.0f, -1.0f, 0.0f)) == float3(0.0f, 1.0f, 0.0f));
    REQUIRE(float3_abs(float3(0.0f, 0.0f, -1.0f)) == float3(0.0f, 0.0f, 1.0f));
}

TEST_CASE("float3 - argmax", "[float3]") {
    using namespace fb;
    REQUIRE(float3_argmax(float3(0.0f, 0.0f, 0.0f)) == 0u);
    REQUIRE(float3_argmax(float3(1.0f, 0.0f, 0.0f)) == 0u);
    REQUIRE(float3_argmax(float3(0.0f, 1.0f, 0.0f)) == 1u);
    REQUIRE(float3_argmax(float3(0.0f, 0.0f, 1.0f)) == 2u);
}

//
// Setup.
//

auto main() -> int {
    Catch::ConfigData config_data;
    config_data.testsOrTags.push_back("*");
    config_data.showSuccessfulTests = true;
    Catch::Session session;
    session.useConfigData(config_data);
    const auto tests_failed = session.run();
    return tests_failed != 0;
}
