
#pragma once
namespace stamping
{
#ifndef ESP8266
static const int upPin{7};
static const int downPin{4};
static const int selectPin{5};
static const int backPin{6};
static const int a1Pin{3};
static const int a2Pin{2};
static const int a3Pin{8};
static const int a4Pin{9};
static const int sdSelectPin{10};
static const int programReadyPin{LED_BUILTIN};
#else
static const int a1Pin{15};
static const int a2Pin{2};
static const int a3Pin{13};
static const int a4Pin{12};
//static const int sdSelectPin{2};
#endif
}
