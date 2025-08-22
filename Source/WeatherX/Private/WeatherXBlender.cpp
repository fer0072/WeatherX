#include "WeatherXBlender.h"
#include "WeatherXDataPacket.h"


UWeatherXDataPacket* UWeatherXBlender::CollectDataPacket()
{
	return NewObject<UWeatherXDataPacket>();
}