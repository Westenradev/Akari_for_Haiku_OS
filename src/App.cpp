#include "App.h"

#include <Deskbar.h>
#include <Message.h>

#include "AkariWindow.h"
#include "ReplicantView.h"
#include "TintController.h"

namespace akari {

const char* kAkariSignature = "application/x-vnd.WestenraSoftworks-Akari";


App::App()
	:
	BApplication(kAkariSignature),
	fController(NULL),
	fWindow(NULL),
	fReplicant(NULL),
	fDeskbarItemId(-1)
{
	fSettings.Load();
	fController = new TintController();
	fController->Init();
	fController->LoadSettings(fSettings.fData);
}


App::~App()
{
	delete fController;
}


void
App::ReadyToRun()
{
	fWindow = new AkariWindow(fController, fSettings.fData);
	AddReplicant();
	NotifyReplicant();
}


void
App::MessageReceived(BMessage* message)
{
	switch (message->what) {
			case kMsgReplicantToggle:
		{
			bool enabled = fController->Settings().enabled;
			fController->SetEnabled(!enabled);
			fController->SaveSettings();
			if (fWindow != NULL)
				fWindow->PostMessage(kMsgTick);
			NotifyReplicant();
			break;
		}
		default:
			BApplication::MessageReceived(message);
			break;
	}
}



bool
App::QuitRequested()
{
	if (fDeskbarItemId >= 0) {
		BDeskbar deskbar;
		deskbar.RemoveItem(fDeskbarItemId);
	}
	fController->SaveSettings();
	return true;
}


void
App::AddReplicant()
{
	fReplicant = new ReplicantView(BRect(0, 0, 31, 31));

	BMessage archive;
	if (fReplicant->Archive(&archive) == B_OK) {
		BDeskbar deskbar;
		deskbar.AddItem(fReplicant, &fDeskbarItemId);
	} else {
		delete fReplicant;
		fReplicant = NULL;
	}
}


void
App::NotifyReplicant()
{
	if (fReplicant == NULL)
		return;
	BMessage message(kMsgReplicantState);
	message.AddBool("lit", fController->IsTintActive());
	BMessenger messenger(NULL, fReplicant->Window());
	if (!messenger.IsValid())
		messenger = BMessenger(fReplicant);
	if (messenger.IsValid())
		messenger.SendMessage(&message);
}

}
