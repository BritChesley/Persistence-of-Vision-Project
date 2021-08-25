//#include <SFML/Graphics.hpp>
//
//#include <windows.h>
//#include "ftd2xx.h"
//#include <stdio.h>
//#include <stdint.h>
//#include "ftdi.h"
//#include "libMPSSE_spi.h"
//#include <fstream>
//#include <iostream>
//
//#define IMAGE_SIZE (11*1024) //bytes
//
//int main()
//{
//
//	sf::RenderWindow window(sf::VideoMode(800, 800),
//		"Persistence of Vision Display", sf::Style::Default);
//
//	sf::Font font;
//	if (!font.loadFromFile("arial.ttf"))
//	{
//		std::cout << "can't load font" << std::endl;
//	}
//
//	sf::Text text1;
//	text1.setCharacterSize(30);
//	text1.setOrigin(0, 15);
//	text1.setFont(font);
//	text1.setPosition(200, 100);
//	text1.setFillColor(sf::Color::Black);
//	text1.setString("Image 1");
//
//	sf::Text text2;
//	text2.setCharacterSize(30);
//	text2.setOrigin(0, 15);
//	text2.setFont(font);
//	text2.setPosition(200, 300);
//	text2.setFillColor(sf::Color::Black);
//	text2.setString("Image 2");
//
//	sf::Text text3;
//	text3.setCharacterSize(30);
//	text3.setOrigin(0, 15);
//	text3.setFont(font);
//	text3.setPosition(200, 500);
//	text3.setFillColor(sf::Color::Black);
//	text3.setString("GIF");
//
//	sf::Text text4;
//	text4.setCharacterSize(30);
//	text4.setOrigin(0, 15);
//	text4.setFont(font);
//	text4.setPosition(200, 700);
//	text4.setFillColor(sf::Color::Black);
//	text4.setString("Sound");
//
//	sf::CircleShape circle(20);
//	circle.setOrigin(20, 20);
//	circle.setFillColor(sf::Color(255, 255, 255));
//	circle.setOutlineThickness(-5);
//	circle.setOutlineColor(sf::Color(0, 0, 0));
//	circle.setPosition(100, 100);
//
//	sf::RectangleShape rectangle1(sf::Vector2f(800, 200));
//	rectangle1.setFillColor(sf::Color(255, 255, 255));
//	rectangle1.setOutlineThickness(-20);
//	rectangle1.setOutlineColor(sf::Color(0, 0, 0));
//
//	sf::RectangleShape rectangle2(sf::Vector2f(800, 200));
//	rectangle2.setFillColor(sf::Color(255, 255, 255));
//	rectangle2.setOutlineThickness(-20);
//	rectangle2.setOutlineColor(sf::Color(0, 0, 0));
//	rectangle2.setPosition(0, 200);
//
//	sf::RectangleShape rectangle3(sf::Vector2f(800, 200));
//	rectangle3.setFillColor(sf::Color(255, 255, 255));
//	rectangle3.setOutlineThickness(-20);
//	rectangle3.setOutlineColor(sf::Color(0, 0, 0));
//	rectangle3.setPosition(0, 400);
//
//	sf::RectangleShape rectangle4(sf::Vector2f(800, 200));
//	rectangle4.setFillColor(sf::Color(255, 255, 255));
//	rectangle4.setOutlineThickness(-20);
//	rectangle4.setOutlineColor(sf::Color(0, 0, 0));
//	rectangle4.setPosition(0, 600);
//
//
//
//
//	uint16_t animation[8][20 * 1024];
//	uint16_t data[30 * 1024]; //22KB data
//	uint16_t data2[30 * 1024];
//	/* Open file and read it */
//	std::ifstream inFile;
//	inFile.open("usbtest.txt");
//	if (!inFile)
//	{
//		printf("Error opening image file");
//		getchar();
//		return 1;
//	}
//	int file_len = 0;
//	int file_len2 = 0;
//	int count2 = 0;
//	//read from file
//	while (inFile >> std::hex >> data[file_len++]);
//
//
//	//close file
//	inFile.close();
//
//	/****************** Animation Test ********************/
//
//
//	inFile.open("0.txt");
//	if (!inFile)
//	{
//		printf("Error opening image file0");
//		getchar();
//		return 1;
//	}
//	uint32_t ani_len = 0;
//	while (inFile >> std::hex >> animation[0][ani_len++]);
//	inFile.close();
//
//
//	inFile.open("1.txt");
//	if (!inFile)
//	{
//		printf("Error opening image file1");
//		getchar();
//		return 1;
//	}
//	 ani_len = 0;
//	while (inFile >> std::hex >> animation[1][ani_len++]);
//	inFile.close();
//
//
//	inFile.open("2.txt");
//	if (!inFile)
//	{
//		printf("Error opening image file2");
//		getchar();
//		return 1;
//	}
//	 ani_len = 0;
//	while (inFile >> std::hex >> animation[2][ani_len++]);
//
//
//	inFile.close();
//
//
//	inFile.open("3.txt");
//	if (!inFile)
//	{
//		printf("Error opening image file3");
//		getchar();
//		return 1;
//	}
//	ani_len = 0;
//	while (inFile >> std::hex >> animation[3][ani_len++]);
//
//	inFile.close();
//
//
//	inFile.open("4.txt");
//	if (!inFile)
//	{
//		printf("Error opening image file4");
//		getchar();
//		return 1;
//	}
//	ani_len = 0;
//	while (inFile >> std::hex >> animation[4][ani_len++]);
//
//	inFile.close();
//
//
//	inFile.open("5.txt");
//	if (!inFile)
//	{
//		printf("Error opening image file5");
//		getchar();
//		return 1;
//	}
//	ani_len = 0;
//	while (inFile >> std::hex >> animation[5][ani_len++]);
//
//	inFile.close();
//
//
//	inFile.open("6.txt");
//	if (!inFile)
//	{
//		printf("Error opening image file6");
//		getchar();
//		return 1;
//	}
//	ani_len = 0;
//	while (inFile >> std::hex >> animation[6][ani_len++]);
//
//	inFile.close();
//
//
//	inFile.open("7.txt");
//	if (!inFile)
//	{
//		printf("Error opening image file7");
//		getchar();
//		return 1;
//	}
//	ani_len = 0;
//	while (inFile >> std::hex >> animation[7][ani_len++]);
//
//
//
//	int ani_count = 0;
//	uint8_t ani_ims[8][20 * 1024];
//	for (uint32_t j = 0; j < 8; j++)
//	{
//		ani_count = 0;
//		for (uint32_t i = 0; i < (ani_len); i++)
//		{
//			ani_ims[j][ani_count++] = (uint8_t)((animation[j][i] >> 8) & 0xFF);
//			ani_ims[j][ani_count++] = (uint8_t)(animation[j][i] & 0xFF);
//		}
//	}
//
//	ani_count-= 2;
//
//	//read data now needs to be converted to uint8_t
//	uint8_t image_data[30 * 1024];
//	int count = 0;
//	for (uint32_t i = 0; i < (file_len); i++)
//	{
//		image_data[count++] = (uint8_t)((data[i] >> 8) & 0xFF);
//		image_data[count++] = (uint8_t)(data[i] & 0xFF);
//	}
//	file_len = count;
//
//	FT_HANDLE ftHandle;
//	FT_STATUS ftStatus;
//	DWORD numBytesToRead;
//	DWORD numBytesRead;
//	DWORD numWordsInputBuff;
//	DWORD numWordsRead;
//	int8_t command = 0;
//
//
//
//
//	//uint8_t InputBuffer[BUFFER_SIZE] = { 0 };
//	//uint8_t OutputBuffer[BUFFER_SIZE] = { 0 };
//	//open USB device
// //	ftStatus = FT_OpenEx((PVOID)"FT3MDGEO", FT_OPEN_BY_SERIAL_NUMBER, &ftHandle);
//	//if (ftStatus != FT_OK) // Check if Open was successful
//	//{
//	//	printf("Can't open FT232H device! \n");
//	//	getchar();
//	//	return 1;
//	//}
//
//
//	// Reset the FT232H
//	//ftStatus |= FT_ResetDevice(ftHandle);
//
//	// flush USB receive buffer ... Get the number of bytes in the FT232H receive buffer and then read them
//	//ftStatus |= FT_GetQueueStatus(ftHandle, &numBytesToRead);
//	//if ((ftStatus == FT_OK) && (numBytesToRead > 0))
//	//{
//	//	FT_Read(ftHandle, &InputBuffer, numBytesToRead, &numBytesRead);
//	//}
//	//ftStatus |= FT_SetUSBParameters(ftHandle, USB_TRANSFER_SIZE, USB_TRANSFER_SIZE); // Set USB request transfer sizes
//	//ftStatus |= FT_SetChars(ftHandle, false, 0, false, 0); // Disable event/error characters
//	//ftStatus |= FT_SetTimeouts(ftHandle, 5000, 5000); // Set rd/wr timeouts to 5 sec
//	//ftStatus |= FT_SetLatencyTimer(ftHandle, 16); // Latency timer at default 16ms
//	//ftStatus |= FT_SetBitMode(ftHandle, 0x0, 0x00); // Reset mode to setting in EEPROM
//	//ftStatus |= FT_SetBitMode(ftHandle, 0x0, 0x02); // Enable MPSSE mode
//
//	//if (ftStatus != FT_OK)
//	//{
//	//	printf("failure to initialize FT232H device! \n");
//	//	getchar();
//	//	return 1;
//	//}
//	Cleanup_libMPSSE();
//	Init_libMPSSE();
//	FT_DEVICE_LIST_INFO_NODE devList;
//	ChannelConfig_t config;
//	uint32 channels;
//
//	ftStatus = SPI_GetNumChannels(&channels);
//	if (ftStatus != FT_OK)
//	{
//		printf("Error getting number of spi channels");
//		getchar();
//		return 1;
//	}
//	int32_t channelNum = -1;
//	for (uint32 i = 0; i < channels; i++)
//	{
//		ftStatus = SPI_GetChannelInfo(i, &devList);
//		if (ftStatus != FT_OK)
//		{
//			printf("error enumerating channels");
//			getchar();
//			return 1;
//		}
//
//		if (strcmp(devList.SerialNumber, "FT3GXJ4E") == 0)
//		{
//			channelNum = i;
//		}
//
//	}
//	ftStatus = SPI_OpenChannel(channelNum, &ftHandle);
//	if (ftStatus != FT_OK)
//	{
//		printf("Error opening channel");
//		getchar();
//		return 1;
//	}
//
//	config.ClockRate = 10000000; //100 KHz clock rate currently
//	config.LatencyTimer = 500; //16ms latency timer
//	//cs ative low, cs on DB3, config mode 0 -> sample on rising edge, shifted on falling edge
//	config.configOptions = SPI_CONFIG_OPTION_CS_ACTIVELOW | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_MODE2;
//	config.Pin = (0x09 << 24) | (DEFAULT_PIN_DIRS << 16) | (0x09 << 8) | (DEFAULT_PIN_DIRS);
//	ftStatus = SPI_InitChannel(ftHandle, &config); //configure SPI channel
//
//	//if (ftStatus != FT_OK)
//	//{
//	//	printf("Failure to intialize SPI mode");
//	//	getchar();
//	//	return 1;
//	//}
//
//	uint8_t data_t[5000];
//	uint32_t buffer_len = 5000;
//	uint32 sizeTransferred;
//	uint32_t transferOptions = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE
//		| SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE;
//
//
//
//
//
//
//	///*********************** Send animations *******************/
//	//while (1)
//	//{
//
//
//	//}
//
//
//
//	/***************** SEND FIRST IMAGE ************************/
//
//	//ftStatus = SPI_Write(ftHandle, image_data, count >> 1, &sizeTransferred, transferOptions);
//	////ftStatus = SPI_Write(ftHandle, data_t, buffer_len, &sizeTransferred, transferOptions);
//	//if (ftStatus != FT_OK || count >> 1 != sizeTransferred)
//	//{
//	//	printf("Error writing SPI data");
//	//	getchar();
//	//	return 1;
//	//}
//	////for (int i = 0; i < 10000000; i++);
//	//transferOptions = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE
//	//	| SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;
//
//
//	//ftStatus = SPI_Write(ftHandle, &image_data[5000], count >> 1, &sizeTransferred, transferOptions);
//	//if (ftStatus != FT_OK || count >> 1 != sizeTransferred)
//	//{
//	//	printf("Error writing SPI data");
//	//	getchar();
//	//	return 1;
//	//}
//
//	//printf("Image written to C2000\n");
//	//printf("Press enter to write second image\n");
//	//getchar();
//	////OPEN NEXT IMAGE
//
//	inFile.close();
//	/* Open file and read it */
//	inFile.open("supercheck.txt");
//	if (!inFile)
//	{
//		printf("Error opening image file");
//		getchar();
//		return 1;
//	}
//	file_len2 = 0;
//	//read from file
//	while (inFile >> std::hex >> data2[file_len2++]);
//
//
//
//	////close file
//	inFile.close();
//	count2 = 0;
//	//read data now needs to be converted to uint8_t
//	uint8_t image_data2[30 * 1024];
//	for (uint32_t i = 0; i < (file_len2); i++)
//	{
//		image_data2[count2++] = (uint8_t)((data2[i] >> 8) & 0xFF);
//		image_data2[count2++] = (uint8_t)(data2[i] & 0xFF);
//	}
//	file_len2 = count2;
//
//	///********************* SEND NEXT IMAGE **************************/
//
//	count2 -= 2;
//	//transferOptions = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE
//	//	| SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE;
//	//ftStatus = SPI_Write(ftHandle, image_data2, count2 >> 1, &sizeTransferred, transferOptions);
//	////ftStatus = SPI_Write(ftHandle, data_t, buffer_len, &sizeTransferred, transferOptions);
//	//if (ftStatus != FT_OK || count2 >> 1 != sizeTransferred)
//	//{
//	//	printf("Error writing SPI data");
//	//	getchar();
//	//	return 1;
//	//}
//	////for (int i = 0; i < 10000000; i++);
//	//transferOptions = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE
//	//	| SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;
//	//ftStatus = SPI_Write(ftHandle, &image_data2[5000], count2 >> 1, &sizeTransferred, transferOptions);
//	//if (ftStatus != FT_OK || count2 >> 1 != sizeTransferred)
//	//{
//	//	printf("Error writing SPI data");
//	//	getchar();
//	//	return 1;
//	//}
//
//
//	//printf("Second image written successfully\n");
//
//	//printf("Press enter to change to sound mode\n");
//	//getchar();
//
//	uint8_t sound_data[10] = { 0 };
//	for (uint16_t i = 0; i < 10; i++)
//	{
//		sound_data[i] = i;
//	}
//	//ftStatus = SPI_Write(ftHandle, sound_data, 10, &sizeTransferred, transferOptions);
//	//if (ftStatus != FT_OK || 10 != sizeTransferred)
//	//{
//	//	printf("Error writing SPI data");
//	//	getchar();
//	//	return 1;
//	//}
//
//
//
//	count -= 2;
//	//ftStatus = SPI_Write(ftHandle, image_data, count >> 1, &sizeTransferred, transferOptions);
//	////ftStatus = SPI_Write(ftHandle, data_t, buffer_len, &sizeTransferred, transferOptions);
//	//if (ftStatus != FT_OK || count >> 1 != sizeTransferred)
//	//{
//	//	printf("Error writing SPI data");
//	//	getchar();
//	//	return 1;
//	//}
//	////for (int i = 0; i < 10000000; i++);
//	//transferOptions = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE
//	//	| SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;
//
//
//	//ftStatus = SPI_Write(ftHandle, &image_data[5000], count >> 1, &sizeTransferred, transferOptions);
//	//if (ftStatus != FT_OK || count >> 1 != sizeTransferred)
//	//{
//	//	printf("Error writing SPI data");
//	//	getchar();
//	//	return 1;
//	//}
//
//	//printf("Image written to C2000\n");
//	//printf("Press enter to write second image\n");
//	//getchar();
//	//OPEN NEXT IMAGE
//
//	inFile.close();
//	/* Open file and read it */
//	inFile.open("supercheck.txt");
//	if (!inFile)
//	{
//		printf("Error opening image file");
//		getchar();
//		return 1;
//	}
//	file_len2 = 0;
//	//read from file
//	while (inFile >> std::hex >> data2[file_len2++]);
//
//
//
//	//close file
//	inFile.close();
//	//count2 = 0;
//	//read data now needs to be converted to uint8_t
//	//uint8_t image_data2[30 * 1024];
//	//for (uint32_t i = 0; i < (file_len); i++)
//	//{
//	//	image_data2[count2++] = (uint8_t)((data2[i] >> 8) & 0xFF);
//	//	image_data2[count2++] = (uint8_t)(data2[i] & 0xFF);
//	//}
//	//file_len2 = count2;
//
//	///********************* SEND NEXT IMAGE **************************/
//
//	//count2 -= 2;
//	//transferOptions = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE
//	//	| SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE;
//	//ftStatus = SPI_Write(ftHandle, image_data2, count2 >> 1, &sizeTransferred, transferOptions);
//	////ftStatus = SPI_Write(ftHandle, data_t, buffer_len, &sizeTransferred, transferOptions);
//	//if (ftStatus != FT_OK || count2 >> 1 != sizeTransferred)
//	//{
//	//	printf("Error writing SPI data");
//	//	getchar();
//	//	return 1;
//	//}
//	////for (int i = 0; i < 10000000; i++);
//	//transferOptions = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE
//	//	| SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;
//	//ftStatus = SPI_Write(ftHandle, &image_data2[5000], count2 >> 1, &sizeTransferred, transferOptions);
//	//if (ftStatus != FT_OK || count2 >> 1 != sizeTransferred)
//	//{
//	//	printf("Error writing SPI data");
//	//	getchar();
//	//	return 1;
//	//}
//
//
//	//printf("Second image written successfully\n");
//
//	//printf("Press enter to change to sound mode\n");
//	//getchar();
//
//	//uint8_t sound_data[10] = { 0 };
//	//for (uint16_t i = 0; i < 10; i++)
//	//{
//	//	sound_data[i] = i;
//	//}
//	//ftStatus = SPI_Write(ftHandle, sound_data, 10, &sizeTransferred, transferOptions);
//	//if (ftStatus != FT_OK || 10 != sizeTransferred)
//	//{
//	//	printf("Error writing SPI data");
//	//	getchar();
//	//	return 1;
//	//}
//
//	while (1)
//	{
//
//
//
//		while (window.isOpen())
//		{
//			window.clear();
//			window.draw(rectangle1);
//			window.draw(rectangle2);
//			window.draw(rectangle3);
//			window.draw(rectangle4);
//			window.draw(text1);
//			window.draw(text2);
//			window.draw(text3);
//			window.draw(text4);
//			window.draw(circle);
//			window.display();
//			sf::Event event;
//			while (window.pollEvent(event))
//			{
//				if (event.type == sf::Event::Closed)
//				{
//					window.close();
//				}
//				//Changes difficulty
//				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
//				{
//
//					if (command > 0)
//					{
//						circle.move(0, -200);
//						command--;
//
//					}
//
//				}
//				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
//
//				{
//					if (command < 3)
//					{
//						circle.move(0, 200);
//						command++;
//					}
//
//				}
//				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
//				{
//					//send command
//					//window.close();
//					if (command == 0)
//					{
//						//send first image
//						transferOptions = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE
//							| SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE;
//						ftStatus = SPI_Write(ftHandle, image_data, count >> 1, &sizeTransferred, transferOptions);
//						//ftStatus = SPI_Write(ftHandle, data_t, buffer_len, &sizeTransferred, transferOptions);
//						if (ftStatus != FT_OK || count >> 1 != sizeTransferred)
//						{
//							printf("Error writing SPI data");
//							getchar();
//							return 1;
//						}
//						//for (int i = 0; i < 10000000; i++);
//						transferOptions = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE
//							| SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;
//
//
//						ftStatus = SPI_Write(ftHandle, &image_data[5000], count >> 1, &sizeTransferred, transferOptions);
//						if (ftStatus != FT_OK || count >> 1 != sizeTransferred)
//						{
//							printf("Error writing SPI data");
//							getchar();
//							return 1;
//						}
//
//					}
//					else if (command == 1)
//					{
//						//send second image
//						transferOptions = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE
//							| SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE;
//						ftStatus = SPI_Write(ftHandle, image_data2, count2 >> 1, &sizeTransferred, transferOptions);
//						if (ftStatus != FT_OK || count2 >> 1 != sizeTransferred)
//						{
//							printf("Error writing SPI data");
//							getchar();
//							return 1;
//						}
//						//for (int i = 0; i < 10000000; i++);
//						transferOptions = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE
//							| SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;
//						ftStatus = SPI_Write(ftHandle, &image_data2[5000], count2 >> 1, &sizeTransferred, transferOptions);
//						if (ftStatus != FT_OK || count2 >> 1 != sizeTransferred)
//						{
//							printf("Error writing SPI data");
//							getchar();
//							return 1;
//						}
//					}
//					else if (command == 2)
//					{
//						do
//						{
//							//send GIF
//							for (uint8_t i = 0; i < 8 && (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space)); i++)
//							{
//
//								transferOptions = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE
//									| SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE;
//
//								ftStatus = SPI_Write(ftHandle, ani_ims[i], ani_count >> 1, &sizeTransferred, transferOptions);
//								//ftStatus = SPI_Write(ftHandle, data_t, buffer_len, &sizeTransferred, transferOptions);
//								if (ftStatus != FT_OK || ani_count >> 1 != sizeTransferred)
//								{
//									printf("Error writing SPI data");
//									std::cout << i;
//									getchar();
//									return 1;
//								}
//								//for (int i = 0; i < 10000000; i++);
//								transferOptions = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE
//									| SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;
//
//
//								ftStatus = SPI_Write(ftHandle, &ani_ims[i][5000], ani_count >> 1, &sizeTransferred, transferOptions);
//								if (ftStatus != FT_OK || ani_count >> 1 != sizeTransferred)
//								{
//									printf("Error writing SPI data");
//									std::cout << i;
//									getchar();
//									return 1;
//								}
//
//								Sleep(125); //sleep 100 ms
//
//							}
//							window.pollEvent(event);
//
//							window.clear();
//							window.draw(rectangle1);
//							window.draw(rectangle2);
//							window.draw(rectangle3);
//							window.draw(rectangle4);
//							window.draw(text1);
//							window.draw(text2);
//							window.draw(text3);
//							window.draw(text4);
//							window.draw(circle);
//							window.display();
//
//
//						} while ((!sf::Keyboard::isKeyPressed(sf::Keyboard::Space)));
//
//					}
//					else
//					{
//
//						//enter sound mode
//						transferOptions = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE
//							| SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;
//						ftStatus = SPI_Write(ftHandle, sound_data, 10, &sizeTransferred, transferOptions);
//						if (ftStatus != FT_OK || 10 != sizeTransferred)
//						{
//							printf("Error writing SPI data");
//							getchar();
//							return 1;
//						}
//					}
//
//
//
//				}
//			}
//		}
//	}
//
//
//	///*	printf("Press enter to write the first image\n");
//	//	getchar();*/
//
//	//	ftStatus = SPI_Write(ftHandle, image_data, count >> 1, &sizeTransferred, transferOptions);
//	//	//ftStatus = SPI_Write(ftHandle, data_t, buffer_len, &sizeTransferred, transferOptions);
//	//	if (ftStatus != FT_OK || count >> 1 != sizeTransferred)
//	//	{
//	//		printf("Error writing SPI data");
//	//		getchar();
//	//		return 1;
//	//	}
//	//	//for (int i = 0; i < 10000000; i++);
//	//	transferOptions = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE
//	//		| SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;
//
//
//	//	ftStatus = SPI_Write(ftHandle, &image_data[5000], count >> 1, &sizeTransferred, transferOptions);
//	//	if (ftStatus != FT_OK || count >> 1 != sizeTransferred)
//	//	{
//	//		printf("Error writing SPI data");
//	//		getchar();
//	//		return 1;
//	//	}
//
//	//	printf("Press enter to send second image\n");
//	//	getchar();
//
//	//	transferOptions = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE
//	//		| SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE;
//	//	ftStatus = SPI_Write(ftHandle, image_data2, count2 >> 1, &sizeTransferred, transferOptions);
//	//	//ftStatus = SPI_Write(ftHandle, data_t, buffer_len, &sizeTransferred, transferOptions);
//	//	if (ftStatus != FT_OK || count2 >> 1 != sizeTransferred)
//	//	{
//	//		printf("Error writing SPI data");
//	//		getchar();
//	//		return 1;
//	//	}
//	//	//for (int i = 0; i < 10000000; i++);
//	//	transferOptions = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE
//	//		| SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;
//	//	ftStatus = SPI_Write(ftHandle, &image_data2[5000], count2 >> 1, &sizeTransferred, transferOptions);
//	//	if (ftStatus != FT_OK || count2 >> 1 != sizeTransferred)
//	//	{
//	//		printf("Error writing SPI data");
//	//		getchar();
//	//		return 1;
//	//	}
//
//	//	printf("Press enter to change to sound mode\n");
//	//	getchar();
//	//	ftStatus = SPI_Write(ftHandle, sound_data, 10, &sizeTransferred, transferOptions);
//	//	if (ftStatus != FT_OK || 10 != sizeTransferred)
//	//	{
//	//		printf("Error writing SPI data");
//	//		getchar();
//	//		return 1;
//	//	}
//
//	//	printf("Press enter to send a GIF\n");
//	//	getchar();
//	//	printf("");
//	//	char input = 'a';
//	//	while (&input != "\n")
//	//	{
//	//		std::cin >> input;
//	//		for (uint8_t i = 0; i < 8; i++)
//	//		{
//
//	//			transferOptions = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE
//	//				| SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE;
//
//	//			ftStatus = SPI_Write(ftHandle, ani_ims[i], ani_count >> 1, &sizeTransferred, transferOptions);
//	//			//ftStatus = SPI_Write(ftHandle, data_t, buffer_len, &sizeTransferred, transferOptions);
//	//			if (ftStatus != FT_OK || ani_count >> 1 != sizeTransferred)
//	//			{
//	//				printf("Error writing SPI data");
//	//				std::cout << i;
//	//				getchar();
//	//				return 1;
//	//			}
//	//			//for (int i = 0; i < 10000000; i++);
//	//			transferOptions = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE
//	//				| SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;
//
//
//	//			ftStatus = SPI_Write(ftHandle, &ani_ims[i][5000], ani_count >> 1, &sizeTransferred, transferOptions);
//	//			if (ftStatus != FT_OK || ani_count >> 1 != sizeTransferred)
//	//			{
//	//				printf("Error writing SPI data");
//	//				std::cout << i;
//	//				getchar();
//	//				return 1;
//	//			}
//
//	//			Sleep(125); //sleep 100 ms
//
//
//	//		}
//	//	}
//
//
//
//
//	//	Sleep(50);
//	//}
//
//
//}