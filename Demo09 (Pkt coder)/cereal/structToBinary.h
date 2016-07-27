#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>

namespace Binary
{
	struct NPCInfo
	{
		short MP = 202;
		short HP = 102;

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(MP, HP);
		}
	};

	struct StructToBinary1
	{
		NPCInfo TestNPC;

		void EncodingDecoding()
		{
			std::cout << "StructToBinary1 - Encoding Start" << std::endl;

			std::stringstream ss;
			{
				cereal::BinaryOutputArchive o_archive(ss);
				o_archive(TestNPC);
			}
			std::cout << ss.str() << ss.str().size() << std::endl;
			std::cout << "StructToBinary1 - Encoding End" << std::endl;
			std::cout << std::endl;

			std::cout << "StructToBinary1 - Decoding Start" << std::endl;
			NPCInfo testNPC;
			cereal::BinaryInputArchive i_archive(ss);
			i_archive(testNPC);

			std::cout << testNPC.MP << std::endl;
			std::cout << testNPC.HP << std::endl;
			std::cout << "StructToBinary1 - Decoding End" << std::endl;
			std::cout << std::endl;
		}

	};


	struct StructToBinary2
	{
		NPCInfo TestNPC;

		int BufferDatzSize = 0;
		char Buffer[256] = { 0, };

		void Encoding()
		{
			std::cout << "StructToBinary2 - Encoding Start" << std::endl;

			std::stringstream ss;
			{
				cereal::BinaryOutputArchive o_archive(ss);
				o_archive(TestNPC);
			}
		
			BufferDatzSize = ss.str().size();
			memcpy(Buffer, ss.str().data(), BufferDatzSize);
		}

		void Decoding()
		{
			std::cout << "StructToBinary2 - Decoding Start" << std::endl;
			
			
			NPCInfo testNPC;

			std::string input;
			input.assign(Buffer, BufferDatzSize);

			std::istringstream ss(input);
			{
				cereal::BinaryInputArchive i_archive(ss);
				i_archive(testNPC);
			}
			
			std::cout << testNPC.MP << std::endl;
			std::cout << testNPC.HP << std::endl;
			std::cout << "StructToBinary2 - Decoding End" << std::endl;
			std::cout << std::endl;
		}

	};
}
