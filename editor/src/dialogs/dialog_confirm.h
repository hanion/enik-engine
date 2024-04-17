#pragma once
#include <base.h>

namespace Enik {

struct DialogConfirmData {
	bool is_open = false;
	std::string text;
	std::string description;

	std::function<void()> call_function;
};

class DialogConfirm {
public:
	enum DialogConfirmResult {
		CANCEL = 0,
		CONFIRM = 1
	};

public:
	static void OpenDialog(const std::string& text, const std::function<void()>& call_when_confirmed);
	static void OpenDialog(const std::string& text, const std::string& description, const std::function<void()>& call_when_confirmed);


	// ? This is a BeginPopupModal
	// ? Call OpenDialog once before
	static DialogConfirmResult Show();

private:
	static DialogConfirmResult ShowPopup();
};

}