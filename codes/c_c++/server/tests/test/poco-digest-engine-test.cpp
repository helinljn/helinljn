#include "gtest/gtest.h"
#include "util/types.h"
#include "Poco/String.h"
#include "Poco/MD4Engine.h"
#include "Poco/MD5Engine.h"
#include "Poco/SHA1Engine.h"
#include "Poco/SHA2Engine.h"
#include "Poco/HMACEngine.h"

GTEST_TEST(PocoDigestEngineTest, MD4)
{
    Poco::MD4Engine engine;

    engine.update("");
    ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "31d6cfe0d16ae931b73c59d7e0c089c0");

    engine.update('a');
    ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "bde52cb31de33e46245e05fbdbd6fb24");

    engine.update("message digest");
    ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "d9130a8164549fe818874806e1c7014b");

    engine.update("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    engine.update("abcdefghijklmnopqrstuvwxyz0123456789");
    ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "043f8582f241db351ce627e153e7f0e4");

    const auto d1 = Poco::DigestEngine::digestFromHex("31d6cfe0d16ae931b73c59d7e0c089c0");
    const auto d2 = Poco::DigestEngine::digestFromHex("31d6cfe0d16ae931b73c59d7e0c089c0");
    const auto d3 = Poco::DigestEngine::digestFromHex("bde52cb31de33e46245e05fbdbd6fb24");

    ASSERT_TRUE(Poco::DigestEngine::constantTimeEquals(d1, d2));
    ASSERT_TRUE(!Poco::DigestEngine::constantTimeEquals(d1, d3));
}

GTEST_TEST(PocoDigestEngineTest, MD5)
{
    Poco::MD5Engine engine;

    engine.update("");
    ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "d41d8cd98f00b204e9800998ecf8427e");

    engine.update('a');
    ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "0cc175b9c0f1b6a831c399e269772661");

    engine.update("message digest");
    ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "f96b697d7cb7938d525a2f31aaf161d0");

    engine.update("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    engine.update("abcdefghijklmnopqrstuvwxyz0123456789");
    ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "d174ab98d277d9f5a5611c2c9f419d9f");

    const auto d1 = Poco::DigestEngine::digestFromHex("d41d8cd98f00b204e9800998ecf8427e");
    const auto d2 = Poco::DigestEngine::digestFromHex("d41d8cd98f00b204e9800998ecf8427e");
    const auto d3 = Poco::DigestEngine::digestFromHex("0cc175b9c0f1b6a831c399e269772661");

    ASSERT_TRUE(Poco::DigestEngine::constantTimeEquals(d1, d2));
    ASSERT_TRUE(!Poco::DigestEngine::constantTimeEquals(d1, d3));
}

GTEST_TEST(PocoDigestEngineTest, SHA1)
{
    Poco::SHA1Engine engine;

    engine.update("");
    ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "da39a3ee5e6b4b0d3255bfef95601890afd80709");

    engine.update('a');
    ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "86f7e437faa5a7fce15d1ddcb9eaeaea377667b8");

    engine.update("message digest");
    ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "c12252ceda8be8994d5fa0290a47231c1d16aae3");

    engine.update("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    engine.update("abcdefghijklmnopqrstuvwxyz0123456789");
    ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "761c457bf73b14d27e9e9265c46f4b4dda11f940");

    const auto d1 = Poco::DigestEngine::digestFromHex("da39a3ee5e6b4b0d3255bfef95601890afd80709");
    const auto d2 = Poco::DigestEngine::digestFromHex("da39a3ee5e6b4b0d3255bfef95601890afd80709");
    const auto d3 = Poco::DigestEngine::digestFromHex("86f7e437faa5a7fce15d1ddcb9eaeaea377667b8");

    ASSERT_TRUE(Poco::DigestEngine::constantTimeEquals(d1, d2));
    ASSERT_TRUE(!Poco::DigestEngine::constantTimeEquals(d1, d3));
}

GTEST_TEST(PocoDigestEngineTest, SHA2)
{
    // SHA_224
    {
        Poco::SHA2Engine engine(Poco::SHA2Engine::SHA_224);

        engine.update("");
        ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f");

        engine.update('a');
        ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "abd37534c7d9a2efb9465de931cd7055ffdb8879563ae98078d6d6d5");

        engine.update("message digest");
        ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "2cb21c83ae2f004de7e81c3c7019cbcb65b71ab656b22d6d0c39b8eb");

        engine.update("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        engine.update("abcdefghijklmnopqrstuvwxyz0123456789");
        ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "bff72b4fcb7d75e5632900ac5f90d219e05e97a7bde72e740db393d9");

        const auto d1 = Poco::DigestEngine::digestFromHex("d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f");
        const auto d2 = Poco::DigestEngine::digestFromHex("d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f");
        const auto d3 = Poco::DigestEngine::digestFromHex("abd37534c7d9a2efb9465de931cd7055ffdb8879563ae98078d6d6d5");

        ASSERT_TRUE(Poco::DigestEngine::constantTimeEquals(d1, d2));
        ASSERT_TRUE(!Poco::DigestEngine::constantTimeEquals(d1, d3));
    }

    // SHA_256
    {
        Poco::SHA2Engine engine(Poco::SHA2Engine::SHA_256);

        engine.update("");
        ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");

        engine.update('a');
        ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb");

        engine.update("message digest");
        ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "f7846f55cf23e14eebeab5b4e1550cad5b509e3348fbc4efa3a1413d393cb650");

        engine.update("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        engine.update("abcdefghijklmnopqrstuvwxyz0123456789");
        ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "db4bfcbd4da0cd85a60c3c37d3fbd8805c77f15fc6b1fdfe614ee0a7c8fdb4c0");

        const auto d1 = Poco::DigestEngine::digestFromHex("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
        const auto d2 = Poco::DigestEngine::digestFromHex("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
        const auto d3 = Poco::DigestEngine::digestFromHex("ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb");

        ASSERT_TRUE(Poco::DigestEngine::constantTimeEquals(d1, d2));
        ASSERT_TRUE(!Poco::DigestEngine::constantTimeEquals(d1, d3));
    }

    // SHA_384
    {
        Poco::SHA2Engine engine(Poco::SHA2Engine::SHA_384);

        engine.update("");
        ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b");

        engine.update('a');
        ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "54a59b9f22b0b80880d8427e548b7c23abd873486e1f035dce9cd697e85175033caa88e6d57bc35efae0b5afd3145f31");

        engine.update("message digest");
        ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "473ed35167ec1f5d8e550368a3db39be54639f828868e9454c239fc8b52e3c61dbd0d8b4de1390c256dcbb5d5fd99cd5");

        engine.update("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        engine.update("abcdefghijklmnopqrstuvwxyz0123456789");
        ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "1761336e3f7cbfe51deb137f026f89e01a448e3b1fafa64039c1464ee8732f11a5341a6f41e0c202294736ed64db1a84");

        const auto d1 = Poco::DigestEngine::digestFromHex("38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b");
        const auto d2 = Poco::DigestEngine::digestFromHex("38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b");
        const auto d3 = Poco::DigestEngine::digestFromHex("54a59b9f22b0b80880d8427e548b7c23abd873486e1f035dce9cd697e85175033caa88e6d57bc35efae0b5afd3145f31");

        ASSERT_TRUE(Poco::DigestEngine::constantTimeEquals(d1, d2));
        ASSERT_TRUE(!Poco::DigestEngine::constantTimeEquals(d1, d3));
    }

    // SHA_512
    {
        Poco::SHA2Engine engine(Poco::SHA2Engine::SHA_512);

        engine.update("");
        ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e");

        engine.update('a');
        ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "1f40fc92da241694750979ee6cf582f2d5d7d28e18335de05abc54d0560e0f5302860c652bf08d560252aa5e74210546f369fbbbce8c12cfc7957b2652fe9a75");

        engine.update("message digest");
        ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "107dbf389d9e9f71a3a95f6c055b9251bc5268c2be16d6c13492ea45b0199f3309e16455ab1e96118e8a905d5597b72038ddb372a89826046de66687bb420e7c");

        engine.update("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        engine.update("abcdefghijklmnopqrstuvwxyz0123456789");
        ASSERT_TRUE(Poco::DigestEngine::digestToHex(engine.digest()) == "1e07be23c26a86ea37ea810c8ec7809352515a970e9253c26f536cfc7a9996c45c8370583e0a78fa4a90041d71a4ceab7423f19c71b9d5a3e01249f0bebd5894");

        const auto d1 = Poco::DigestEngine::digestFromHex("cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e");
        const auto d2 = Poco::DigestEngine::digestFromHex("cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e");
        const auto d3 = Poco::DigestEngine::digestFromHex("1f40fc92da241694750979ee6cf582f2d5d7d28e18335de05abc54d0560e0f5302860c652bf08d560252aa5e74210546f369fbbbce8c12cfc7957b2652fe9a75");

        ASSERT_TRUE(Poco::DigestEngine::constantTimeEquals(d1, d2));
        ASSERT_TRUE(!Poco::DigestEngine::constantTimeEquals(d1, d3));
    }
}

GTEST_TEST(PocoDigestEngineTest, HMAC)
{
    // HMAC-MD5
    {
        Poco::HMACEngine<Poco::MD5Engine> engine("Mob, mob!");
        engine.update("What do you want?");

        std::string hexDigest = Poco::DigestEngine::digestToHex(engine.digest());
        Poco::toUpperInPlace(hexDigest);
        ASSERT_TRUE(hexDigest == "5F9C7122FB316015E5D228891F8EFFEB");
    }

    // HMAC-SHA1
    {
        Poco::HMACEngine<Poco::SHA1Engine> engine("Mob, mob!");
        engine.update("What do you want?");

        std::string hexDigest = Poco::DigestEngine::digestToHex(engine.digest());
        Poco::toUpperInPlace(hexDigest);
        ASSERT_TRUE(hexDigest == "7948A202672F3D9520FD882261559086EE35CA11");
    }

    // HMAC-SHA256
    {
        Poco::HMACEngine<Poco::SHA2Engine256> engine("Mob, mob!");
        engine.update("What do you want?");

        std::string hexDigest = Poco::DigestEngine::digestToHex(engine.digest());
        Poco::toUpperInPlace(hexDigest);
        ASSERT_TRUE(hexDigest == "A2632E4FD3F719E801396D3B2552C202B11910930A8A439FCE6AD4AFA2C767BE");
    }

    // HMAC-SHA384
    {
        Poco::HMACEngine<Poco::SHA2Engine384> engine("Mob, mob!");
        engine.update("What do you want?");

        std::string hexDigest = Poco::DigestEngine::digestToHex(engine.digest());
        Poco::toUpperInPlace(hexDigest);
        ASSERT_TRUE(hexDigest == "EBF4BFAE30786DFC5F0A32B754C78143C32EF08F94A6F75C39551242A257C8E00094363CCA12DF86F49C196D427B76C2");
    }

    // HMAC-SHA512
    {
        Poco::HMACEngine<Poco::SHA2Engine512> engine("Mob, mob!");
        engine.update("What do you want?");

        std::string hexDigest = Poco::DigestEngine::digestToHex(engine.digest());
        Poco::toUpperInPlace(hexDigest);
        ASSERT_TRUE(hexDigest == "A59D61DFC5CA6C7A9F74FB27FE270963DB09B1B153B39BFAB9D7BFB1A58C7884D0CF21E334CB46B5D5F6EEEC3A02D731CBE56E1F7D33BC277EA917F766BE645F");
    }
}