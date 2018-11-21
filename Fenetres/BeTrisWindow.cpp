#include "BeTrisWindow.h"
#include "BeTrisApp.h"
#include "BeTrisMessagesDefs.h"
#include "BeTrisConstants.h"
#include "CPreferenceFile.h"
#include "BeTrisPlayerItem.h"
#include "CTabView.h"

// on ajoute les .h pour les vues
#include "BeTrisAboutView.h"
#include "BeTrisFieldsView.h"
#include "BeTrisPartyLineView.h"
#include "BeTrisWinnerView.h"
#include "BeTrisSettingView.h"
#include "BeTrisNetworkView.h"
#include "BeTrisBlockFreqView.h"

#include <LayoutBuilder.h>
#include <Messenger.h>
#include <OS.h>

// Acces au looper directement
BeTrisWindow		*g_BeTrisWindow = NULL;

/**** constructeur ****/
BeTrisWindow::BeTrisWindow(BRect frame)
: BWindow(frame,"BeNetTris", B_TITLED_WINDOW,
			B_AUTO_UPDATE_SIZE_LIMITS | B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
{
	// initialiser
	g_BeTrisWindow = this;
	
	/*** on construit la vue support de la tabview ****/
	_pSupportView = new BView("support-view", B_WILL_DRAW);

	_pBeTrisTabView = new CTabView("BeTrisTabView");

	/*** le about ***/	
	_aboutView = new BeTrisAboutView("About");
	_pBeTrisTabView->AddTab(_aboutView);
	/*** le plateau de jeu ***/
	_fieldsView = new BeTrisFieldsView("Fields");
	_pBeTrisTabView->AddTab(_fieldsView);
	/*** l'IRC ***/
	_partyLineView = new BeTrisPartyLineView("PartyLine");
	_pBeTrisTabView->AddTab(_partyLineView);
	/*** la liste des scores ***/
	_winnerView = new BeTrisWinnerView("Winner");
	_pBeTrisTabView->AddTab(_winnerView);
	/*** les preferences ***/
	_settingView = new BeTrisSettingView("Settings");
	_pBeTrisTabView->AddTab(_settingView);
	/*** le reseau (Client/Serveur) ***/
	_networkView = new BeTrisNetworkView("Network");
	_pBeTrisTabView->AddTab(_networkView);
	/*** vue de configuration des blocs ***/
	_blockView = new BeTrisBlockFreqView("Blocks");
	_pBeTrisTabView->AddTab(_blockView);

	_pSupportView->AddChild(_pBeTrisTabView);
	
	BLayoutBuilder::Group<>(this)
		.Add(_pSupportView)
	.End();
}

/**** destructeur ****/
BeTrisWindow::~BeTrisWindow()
{
}

/**** Messaged received ****/
void BeTrisWindow::MessageReceived(BMessage *message)
{
	
	switch(message->what)
	{
	// l'etat de la connexion a changee
	case BETRIS_CONNEXION_CHANGE_MSG:
		{
			int32	currentTab = -1;
			BTab	*tab = NULL;
			
			// trouver le tab courant
			currentTab = _pBeTrisTabView->Selection();
			if(currentTab<0)
				return;
		
			// envoyer le message a la vue de ce Tab
			tab = _pBeTrisTabView->TabAt(currentTab);
			if(tab!=NULL)
				BMessenger(tab->View()).SendMessage(BETRIS_CONNEXION_CHANGE_MSG);
		}
		break;
	// afficher la vue des champ de jeu car une partie vient de commencer
	case BETRIS_DISPLAY_FIELDS_MSG:
		_pBeTrisTabView->Select(1);
		break;
	default:
		BWindow::MessageReceived(message);
	}
}

/**** quitter la fenetre ****/
bool BeTrisWindow::QuitRequested()
{
	CPreferenceFile		*prefs = NULL;

	// acces aux preferences
	prefs = g_BeTrisApp->Preferences();

	// fenetre principale
	if(prefs!=NULL)
		prefs->SetRect(Frame(),"win-position");

	// enregistrer les preferences de l'application
	prefs->Save();
	
	// puis celle des vues
	_networkView->SavePreferences();
	_settingView->SavePreferences();
	_blockView->SavePreferences();
	_winnerView->SavePreferences();
	_partyLineView->SaveBadPlayers();

	// quitter l'application
	be_app->PostMessage(B_QUIT_REQUESTED);

	return(true);
}
