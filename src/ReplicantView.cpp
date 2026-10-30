#include "ReplicantView.h"

#include <Application.h>
#include <Message.h>
#include <TranslatorRoster.h>
#include <TranslationUtils.h>

#include "App.h"

namespace akari {

static const rgb_color kLitTint = {245, 184, 94, 255};
static const rgb_color kUnlitTint = {169, 182, 196, 255};


ReplicantView::ReplicantView(BRect frame)
	:
	BView(frame, "AkariReplicant", B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW),
	fLitIcon(NULL),
	fUnlitIcon(NULL),
	fLit(true),
	fAppMessenger(be_app_messenger)
{
	LoadIcons();
}


ReplicantView::ReplicantView(BMessage* archive)
	:
	BView(archive),
	fLitIcon(NULL),
	fUnlitIcon(NULL),
	fLit(true),
	fAppMessenger(be_app_messenger)
{
	bool lit = true;
	if (archive->FindBool("lit", &lit) == B_OK)
		fLit = lit;
	LoadIcons();
}


ReplicantView::~ReplicantView()
{
	delete fLitIcon;
	delete fUnlitIcon;
}


BArchivable*
ReplicantView::Instantiate(BMessage* archive)
{
	if (!validate_instantiation(archive, "ReplicantView"))
		return NULL;
	return new ReplicantView(archive);
}


status_t
ReplicantView::Archive(BMessage* archive, bool deep) const
{
	status_t status = BView::Archive(archive, deep);
	if (status == B_OK)
		status = archive->AddString("add_on", kAkariSignature);
	if (status == B_OK)
		status = archive->AddString("class", "ReplicantView");
	if (status == B_OK)
		status = archive->AddBool("lit", fLit);
	return status;
}


void
ReplicantView::AttachedToWindow()
{
	BView::AttachedToWindow();
	fAppMessenger = BMessenger(NULL, Window());
	if (!fAppMessenger.IsValid())
		fAppMessenger = be_app_messenger;
	SetViewColor(B_TRANSPARENT_COLOR);
}


void
ReplicantView::Draw(BRect updateRect)
{
	BRect bounds = Bounds();
	SetDrawingMode(B_OP_ALPHA);
	SetHighColor(B_TRANSPARENT_COLOR);
	FillRect(bounds, B_SOLID_HIGH);

	BBitmap* icon = fLit ? fLitIcon : fUnlitIcon;
	if (icon != NULL) {
		BRect iconBounds = icon->Bounds();
		BPoint where((bounds.Width() - iconBounds.Width()) / 2.0f,
			(bounds.Height() - iconBounds.Height()) / 2.0f);
		DrawBitmap(icon, where);
	} else {
		SetHighColor(fLit ? kLitTint : kUnlitTint);
		FillRect(bounds.InsetBySelf(4, 4), B_SOLID_HIGH);
	}
}


void
ReplicantView::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMsgReplicantState:
		{
			bool lit = true;
			if (message->FindBool("lit", &lit) == B_OK)
				SetLit(lit);
			break;
		}
		default:
			BView::MessageReceived(message);
			break;
	}
}


void
ReplicantView::MouseDown(BPoint where)
{
	BMessage message(kMsgReplicantToggle);
	if (fAppMessenger.IsValid())
		fAppMessenger.SendMessage(&message);
	BView::MouseDown(where);
}


void
ReplicantView::SetLit(bool lit)
{
	if (fLit == lit)
		return;
	fLit = lit;
	Invalidate();
}


void
ReplicantView::LoadIcons()
{
	BTranslatorRoster* roster = BTranslatorRoster::Default();
	fLitIcon = BTranslationUtils::GetBitmap('VICN', 101, roster);
}

}
