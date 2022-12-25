#include <fstream>

#include "moderation.h"
#include "helper.h"

#include "json.hpp"

std::string Moderation::Banning::GetFilePath()
{
	std::string str = "banned-ips.json";
	return str;
}

void Moderation::Banning::Ban(UObject* PlayerController)
{
	std::ofstream stream(("banned-ips.json"), std::ios::app);

	if (!stream.is_open())
		return;

	auto PlayerState = Helper::GetPlayerStateFromController(PlayerController);

	auto IP = Helper::GetIPf(PlayerState).ToString();
	auto PlayerName = Helper::GetPlayerName(PlayerController);

	nlohmann::json j;
	j["IP"] = IP;
	j["Username"] = PlayerName;

	stream << j << '\n'; // j.dump(4)

	stream.close();

	std::string KickReason = "You have been banned!";

	std::wstring wstr = std::wstring(KickReason.begin(), KickReason.end());
	FString Reason;
	Reason.Set(wstr.c_str());

	static auto ClientReturnToMainMenu = FindObject<UFunction>("/Script/Engine.PlayerController.ClientReturnToMainMenu");
	PlayerController->ProcessEvent(ClientReturnToMainMenu, &Reason);
	// Reason.Free();
}

void Moderation::Banning::Unban(UObject* PlayerController)
{
	std::ifstream input_file(("banned-ips.json"));

	if (!input_file.is_open())
		return;

	std::vector<std::string> lines;
	std::string line;
	int ipToRemove = -1; // the line
	
	auto PlayerState = Helper::GetPlayerStateFromController(PlayerController);

	auto IP = Helper::GetIPf(PlayerState).ToString();

	while (std::getline(input_file, line))
	{
		lines.push_back(line);
		if (line.find(IP) != std::string::npos)
		{
			ipToRemove = lines.size();
		}
	}

	input_file.close();

	if (ipToRemove != -1)
	{
		std::ofstream stream("banned-ips.json", std::ios::ate);
		for (int i = 0; i < lines.size(); i++)
		{
			if (i != ipToRemove - 1)
				stream << lines[i] << '\n';
		}
	}

	// return ipToRemove != 1;
}

bool Moderation::Banning::IsBanned(UObject* PlayerController)
{
	std::ifstream input_file(("banned-ips.json"));
	std::string line;

	if (!input_file.is_open())
		return false;

	auto PlayerState = Helper::GetPlayerStateFromController(PlayerController);

	auto IP = Helper::GetIPf(PlayerState).ToString();

	while (std::getline(input_file, line))
	{
		if (line.find(IP) != std::string::npos)
		{
			return true;
		}
	}

	return false;
}