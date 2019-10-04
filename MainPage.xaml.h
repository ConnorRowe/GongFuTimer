//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "Timer.h"
#include "MainPage.g.h"

namespace GongFuTimer
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();
	private:
		float StrToF(Platform::String^ str);
		Platform::String^ FormatFloat(float f);
		void Start_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Update();
		void Start();
		Timer teaTimer;
		long ticks;
		int infNumber = 0;
		float targetSeconds = 0.0f;
		Platform::String^ debugText;
		std::chrono::time_point<std::chrono::system_clock>	LastFrameTime;
		double delta;
		Windows::UI::Core::CoreDispatcher^ appdispatcher;

		void Reset_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		Windows::UI::Xaml::Controls::MediaElement^ alarmSound;

	};
}
