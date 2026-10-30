#ifndef AKARI_APP_H
#define AKARI_APP_H

#include <Application.h>

#include "Settings.h"

namespace akari {

extern const char* kAkariSignature;

class AkariWindow;
class ReplicantView;
class TintController;

class App : public BApplication {
public:
						App();
	virtual				~App();

	virtual void		ReadyToRun();
	virtual void		MessageReceived(BMessage* message);
	virtual bool		QuitRequested();

private:
	void				AddReplicant();
	void				NotifyReplicant();

	Settings			fSettings;
	TintController*		fController;
	AkariWindow*		fWindow;
	ReplicantView*		fReplicant;
	int32				fDeskbarItemId;
};

}

#endif
