#pragma once
// http://qiita.com/Ushio@github/items/827cf026dcf74328efb7 , http://qiita.com/usagi/items/8e00f23b2508c98947b3 참고 했습니다.

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <limits>

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>

namespace Json
{
	struct NPCInfo
	{
		std::string Name = "test_npc";
		int HP = 102;

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(Name, HP);
		}
	};

	struct StructToJson1
	{
		NPCInfo TestNPC;

		void EncodingDecoding()
		{
			std::cout << "StructToJson1 - Encoding Start" << std::endl;

			// std::stringstream을 사용하여 .str()로 문자열을 얻기 전에 꼭 cereal::JSONOutputArchive는 파괴 되어야 한다
			// cereal 시리얼라이즈는 RAII 이디엄을 기초로 하고 있어서 파괴 될 때까지 출력이 완전하게 되지 않을 가능성이 있다
			std::stringstream ss;
			{
				cereal::JSONOutputArchive o_archive(ss);
				o_archive(TestNPC);
			}
			std::cout << ss.str() << std::endl;
			std::cout << "StructToJson1 - Encoding End" << std::endl;
			std::cout << std::endl;

			std::cout << "StructToJson1 - Decoding Start" << std::endl;
			NPCInfo testNPC;
			cereal::JSONInputArchive i_archive(ss);
			i_archive(testNPC);

			std::cout << testNPC.Name << std::endl;
			std::cout << testNPC.HP << std::endl;
			std::cout << "StructToJson1 - Decoding End" << std::endl;
			std::cout << std::endl;
		}

	};


	struct StructToJson2
	{
		NPCInfo TestNPC;
		std::string EncodingData;

		void Encoding()
		{
			std::cout << "StructToJson2 - Encoding Start" << std::endl;

			std::stringstream ss;
			{
				cereal::JSONOutputArchive o_archive(ss);
				o_archive(TestNPC);
			}
			std::cout << ss.str() << std::endl;
			std::cout << "StructToJson2 - Encoding End" << std::endl;

			EncodingData = ss.str();

			std::cout << std::endl;
		}

		void Decoding()
		{
			std::cout << "StructToJson2 - Decoding Start" << std::endl;

			std::istringstream is(EncodingData);

			NPCInfo testNPC;
			cereal::JSONInputArchive i_archive(is);
			i_archive(testNPC);

			std::cout << testNPC.Name << std::endl;
			std::cout << testNPC.HP << std::endl;
			std::cout << "StructToJson2 - Decoding End" << std::endl;

			std::cout << std::endl;
		}

	};


	//  이름 붙이기
	struct NPCInfo2
	{
		std::string Name = "test_npc2";
		int HP = 202;

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(Name), CEREAL_NVP(HP));
		}
	};

	struct StructToJson3
	{
		NPCInfo2 TestNPC;

		void EncodingDecoding()
		{
			std::cout << "StructToJson3. key에 이름 붙이기 - Encoding Start" << std::endl;

			std::stringstream ss;
			{
				cereal::JSONOutputArchive o_archive(ss);
				o_archive(cereal::make_nvp("root", TestNPC));
			}
			std::cout << ss.str() << std::endl;
			std::cout << "StructToJson3 - Encoding End" << std::endl;
			std::cout << std::endl;

			std::cout << "StructToJson3 - Decoding Start" << std::endl;
			NPCInfo2 testNPC;
			cereal::JSONInputArchive i_archive(ss);
			i_archive(cereal::make_nvp("root", testNPC));

			std::cout << testNPC.Name << std::endl;
			std::cout << testNPC.HP << std::endl;
			std::cout << "StructToJson3 - Decoding End" << std::endl;
			std::cout << std::endl;
		}
	};


	// 비침입형. 기존 소스에 쉽게 적용 가능 
	struct Vector2 {
		Vector2(float x, float y) { this->x = x; this->y = y; }

		float x = 0.0f;
		float y = 0.0f;
	};

	// 파라메터 타입이 꼭 참조이어야 한다
	template<class Archive>
	void serialize(Archive& archive, Vector2& vec)
	{
		archive(cereal::make_nvp("x", vec.x), cereal::make_nvp("y", vec.y));
	}

	struct NPCInfo3
	{
		std::string Name = "test_npc3";
		int HP = 302;
		Vector2 pos = { 5.0f, 12.40f };

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(Name), CEREAL_NVP(HP), CEREAL_NVP(pos));
		}
	};

	struct StructToJson4
	{
		NPCInfo3 TestNPC;

		void EncodingDecoding()
		{
			std::cout << "StructToJson4. 비침입형 - Encoding Start" << std::endl;

			std::stringstream ss;
			{
				cereal::JSONOutputArchive o_archive(ss);
				o_archive(cereal::make_nvp("root", TestNPC));
			}
			std::cout << ss.str() << std::endl;
			std::cout << "StructToJson4 - Encoding End" << std::endl;
			std::cout << std::endl;

			std::cout << "StructToJson4 - Decoding Start" << std::endl;
			NPCInfo3 testNPC;
			cereal::JSONInputArchive i_archive(ss);
			i_archive(cereal::make_nvp("root", testNPC));

			std::cout << testNPC.Name << std::endl;
			std::cout << testNPC.HP << std::endl;
			std::cout << "StructToJson4 - Decoding End" << std::endl;
			std::cout << std::endl;
		}
	};


	// STL 컨테이너 사용 
	struct NPCInfo4
	{
		std::string Name = "test_npc3";
		int HP = 302;
		std::vector<int> Numbers{ 10, 11, 12 };

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(Name), CEREAL_NVP(HP), CEREAL_NVP(Numbers));
		}
	};

	struct StructToJson5
	{
		NPCInfo4 TestNPC;

		void EncodingDecoding()
		{
			std::cout << "StructToJson5. std::vector 사용 - Encoding Start" << std::endl;

			std::stringstream ss;
			{
				cereal::JSONOutputArchive o_archive(ss);
				o_archive(cereal::make_nvp("root", TestNPC));
			}
			std::cout << ss.str() << std::endl;
			std::cout << "StructToJson5 - Encoding End" << std::endl;
			std::cout << std::endl;

			std::cout << "StructToJson5 - Decoding Start" << std::endl;
			NPCInfo4 testNPC;
			cereal::JSONInputArchive i_archive(ss);
			i_archive(cereal::make_nvp("root", testNPC));

			std::cout << testNPC.Name << std::endl;
			std::cout << testNPC.HP << std::endl;
			std::cout << "StructToJson5 - Decoding End" << std::endl;
			std::cout << std::endl;
		}
	};


	// UINT64
	struct StructToJson6
	{
		static void Encoding()
		{
			std::cout << "StructToJson6. UINT64 사용 - Encoding Start" << std::endl;

			std::stringstream ss;
			{
				cereal::JSONOutputArchive o_archive(ss);
				o_archive(cereal::make_nvp("key", std::numeric_limits< std::uint64_t >::max()));
			}

			std::cout << ss.str() << std::endl;
			std::cout << "StructToJson6 - Encoding End" << std::endl;
			std::cout << std::endl;
		}
	};


	// enum class
	struct StructToJson7
	{
		enum class NPCType : std::uint8_t
		{
			NONE
			, TYPE1
			, TYPE2
		};

		static void Encoding()
		{
			std::cout << "StructToJson7. enum class 사용 - Encoding Start" << std::endl;

			std::stringstream ss;
			{
				cereal::JSONOutputArchive o_archive(ss);
				o_archive(cereal::make_nvp("NPCType", NPCType::TYPE1));
			}

			std::cout << ss.str() << std::endl;
			std::cout << "StructToJson7 - Encoding End" << std::endl;
			std::cout << std::endl;
		}
	};
}