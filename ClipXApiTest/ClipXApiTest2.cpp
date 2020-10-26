#include "pch.h"
#include "CppUnitTest.h"
#include "..\ClipX\ClipX.h"
#include <iostream>
#include <fstream>
#include <windows.h>
#include <thread>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ClipXApiTest2
{
	TEST_CLASS(ClipXApiTest2)
	{
	public:

		TEST_METHOD(GetIdent)
		{
			ClipX dev = ClipX();
			Assert::AreEqual(0, dev.Connect("169.254.1.81"));
			char res;
			dev.GetIdentity(&res);
			int i = 0;
		}

		TEST_METHOD(TestKeepAlive)
		{
			ClipX dev = ClipX();
			Assert::AreEqual(0, dev.Connect("169.254.1.81"));
			Sleep(60000);
/*			dev.SocketOpt();
			std::thread tr(&ClipX::KeepAlive, &dev);
			tr.detach();
			Sleep(60000);
			dev.EndKeepAlive();
			Sleep(10000);
			
			*/
			Assert::AreEqual(0, dev.SDOWrite(0x4380, 1, "3"));
		}
		TEST_METHOD(TestConnect)
		{
			ClipX dev = ClipX();
			Assert::AreEqual(0, dev.Connect("169.254.1.81"));
			Assert::AreEqual(0, dev.Disconnect());
		}
		TEST_METHOD(GetSignalTypeByID)
		{
			char data[40];
			ClipX dev = ClipX();
			int state = dev.GetSignalTypeByID(2, data, 40);
			Assert::IsFalse(strncmp("Field Value", data, strlen(data)));
		}

		TEST_METHOD(GetSignalIDByType) {
			char data[25] = "Calculated Value 1";
			ClipX dev = ClipX();
			int ID = dev.GetSignalIDByType(data);
			Assert::IsTrue(ID == 21);
		}

		TEST_METHOD(GetTransducerTypeByID) {
			char data[40];
			ClipX dev = ClipX();
			dev.GetTransducerTypeByID(13, data, 40);
			Assert::IsFalse(strncmp("Full bridge DC 100mV/V", data, strlen(data)));
		}

		TEST_METHOD(TestSDO)
		{
			ClipX dev = ClipX();
			Assert::AreEqual(0, dev.Connect("169.254.1.81"));
//			Assert::AreEqual(0, dev.SDOWrite(0x4380, 1, "3"));
			char buf[100];

			for (int i = 1; i < 7; i++) {
				_itoa_s(i, buf, 10);
				Assert::AreEqual(0, dev.SDOWrite(0x4428, 5, buf));
				_itoa_s(i + 1, buf, 10);
				Assert::AreEqual(0, dev.SDOWrite(0x4428, 7, buf));
				_itoa_s(i, buf, 10);
				Assert::AreEqual(0, dev.SDOWrite(0x44F1, i+2, buf));
				dev.SDOWrite(0x44F2, i + 2, "einheit");


			}

//			Assert::AreEqual(0, dev.SDORead(0x4380, 1, buf, 100));
//			Assert::AreEqual('3', buf[0]);
			Assert::AreEqual(0, dev.Disconnect());
		}

		TEST_METHOD(TestSlowRead)
		{
			std::ofstream myfile;
			myfile.open("TestLogSlow.txt");
			ClipX dev = ClipX();
			dev.Connect("169.254.1.81");
			myfile << "Connected" << std::endl;                  \
				char buf[2048] = { 0 };
			dev.startMeassurement();
			myfile << "Started Meassurement" << std::endl;
			for (int i = 0; i < 100; i++) {
				//std::cout << dev.AvailibleLines() ;
				double values[7];
				if (dev.ReadNextLine(values) > 0)
				{
					sprintf_s(buf, "  Time: \t %f \t %f \t %f \t %f \t %f \t %f \t %f", values[0], values[1], values[2], values[3], values[4], values[5], values[6]);
					myfile << buf << std::endl;
				}
				\
					Sleep(1000);

			}
			dev.stopMeassurement();
			myfile << "Stopped Meassurement" << std::endl;
			dev.AvailibleLines();
			dev.Disconnect();
			myfile.close();
			//std::cin >> buffer; 

		}
		TEST_METHOD(TestFastRead)
		{
			std::ofstream myfile;
			myfile.open("TestLogFast.txt");
			ClipX dev = ClipX();
			dev.Connect("169.254.1.81");
			myfile << "Connected" << std::endl;                  \
				char buf[4096] = { 0 };
			dev.startMeassurement();
			myfile << "Started Meassurement" << std::endl;
			int pointers[2] = { 0, 0 };
			for (int i = 0; i < 100; i++) {
				//std::cout << dev.AvailibleLines() ;
				double values[7];
				do {
					if (dev.ReadNextLine(values) > 0)
						dev.GetPointers(pointers);
						sprintf_s(buf, "  Time: \t %f \t %f \t %f \t %f \t %f \t %f \t %f \t %d \t %d", values[0], values[1], values[2], values[3], values[4], values[5], values[6], pointers[0], pointers[1]);
					myfile << buf << std::endl;
				} while (dev.AvailibleLines() > 0);
				Sleep(1000);
			}

			dev.stopMeassurement();
			std::cout << "Stopped Meassurement" << std::endl;
			dev.Disconnect();
			myfile.close();
			//std::cin >> buffer; */

		}

	};
}