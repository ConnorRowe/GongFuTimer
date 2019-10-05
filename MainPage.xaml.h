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

		bool isFocused = false;
		Windows::UI::Core::CoreDispatcher^ appdispatcher;

		//Timing vars
		Timer teaTimer;
		std::chrono::high_resolution_clock myClock;
		long ticks;
		int infNumber = 0;
		float targetSeconds = 0.0f;
		std::chrono::time_point<std::chrono::high_resolution_clock>	LastFrameTime;
		double delta;

		//Alarm sound
		Windows::UI::Xaml::Controls::MediaElement^ alarmSound;

		//Debug
		Platform::String^ debugText;

		//Helper functions
		float StrToF(Platform::String^ str);
		Platform::String^ FormatFloat(float f);

		//Main functions
		void Update();
		void Start();

		//Events
		void Reset_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Start_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnActivated(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::WindowActivatedEventArgs ^args);

	};
}
