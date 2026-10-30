#ifndef AKARI_REPLICANT_VIEW_H
#define AKARI_REPLICANT_VIEW_H

#include <Messenger.h>
#include <View.h>

class BBitmap;

namespace akari {

enum {
	kMsgReplicantToggle = 'rTOG',
	kMsgReplicantState = 'rST8'
};

class ReplicantView : public BView {
public:
						ReplicantView(BRect frame);
						ReplicantView(BMessage* archive);
	virtual				~ReplicantView();

	static BArchivable*	Instantiate(BMessage* archive);
	virtual status_t	Archive(BMessage* archive, bool deep = true) const;

	virtual void		AttachedToWindow();
	virtual void		Draw(BRect updateRect);
	virtual void		MessageReceived(BMessage* message);
	virtual void		MouseDown(BPoint where);

	void				SetLit(bool lit);

private:
	void				LoadIcons();

	BBitmap*			fLitIcon;
	BBitmap*			fUnlitIcon;
	bool				fLit;
	BMessenger			fAppMessenger;
};

}

#endif
