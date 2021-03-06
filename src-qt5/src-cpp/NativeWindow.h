//===========================================
//  Lumina-DE source code
//  Copyright (c) 2017, Ken Moore
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
//  This is a container object for setting/announcing changes
//    in a native window's properties.
//    The WM will usually run the "setProperty" function on this object,
//     and any other classes/widgets which watch this window can act appropriatly after-the-fact
//  Non-WM classes should use the "Request" signals to ask the WM to do something, and listen for changes later
//===========================================
#ifndef _LUMINA_DESKTOP_NATIVE_WINDOW_H
#define _LUMINA_DESKTOP_NATIVE_WINDOW_H

#include <QString>
#include <QRect>
#include <QSize>
#include <QObject>
#include <QWindow>
#include <QHash>
#include <QVariant>

class NativeWindow : public QObject{
	Q_OBJECT
public:
	enum State{ S_MODAL, S_STICKY, S_MAX_VERT, S_MAX_HORZ, S_SHADED, S_SKIP_TASKBAR, S_SKIP_PAGER, S_HIDDEN, S_FULLSCREEN, S_ABOVE, S_BELOW, S_ATTENTION };
	enum Type{T_DESKTOP, T_DOCK, T_TOOLBAR, T_MENU, T_UTILITY, T_SPLASH, T_DIALOG, T_DROPDOWN_MENU, T_POPUP_MENU, T_TOOLTIP, T_NOTIFICATION, T_COMBO, T_DND, T_NORMAL };
	enum Action {A_MOVE, A_RESIZE, A_MINIMIZE, A_SHADE, A_STICK, A_MAX_VERT, A_MAX_HORZ, A_FULLSCREEN, A_CHANGE_DESKTOP, A_CLOSE, A_ABOVE, A_BELOW};

	enum Property{ 	 /*QVariant Type*/
		None=0, 		/*null*/
		MinSize=1,  		/*QSize*/
		MaxSize=2, 		/*QSize*/
		Size=3, 			/*QSize*/
		GlobalPos=4,	/*QPoint*/
		Title=5, 		/*QString*/
		ShortTitle=6,	/*QString*/
		Icon=7, 		/*QIcon*/
		Name=8, 		/*QString*/
		Workspace=9,	/*int*/
		States=10,		/*QList<NativeWindow::State> : Current state of the window */
		WinTypes=11,	/*QList<NativeWindow::Type> : Current type of window (typically does not change)*/
		WinActions=12, 	/*QList<NativeWindow::Action> : Current actions that the window allows (Managed/set by the WM)*/
		FrameExtents=13, 	/*QList<int> : [Left, Right, Top, Bottom] in pixels */
		RelatedWindows=14, /* QList<WId> - better to use the "isRelatedTo(WId)" function instead of reading this directly*/
		Active=15, 		/*bool*/
		Visible=16 		/*bool*/
		};

	static QList<NativeWindow::Property> allProperties(){
	  //Return all the available properties (excluding "None" and "FrameExtents" (WM control only) )
	  QList<NativeWindow::Property> props;
	  props << MinSize << MaxSize << Size << GlobalPos << Title << ShortTitle << Icon << Name << Workspace \
	    << States << WinTypes << WinActions << RelatedWindows << Active << Visible;
	  return props;
	};

	NativeWindow(WId id);
	~NativeWindow();

	void addFrameWinID(WId);
	void addDamageID(unsigned int);
	bool isRelatedTo(WId);

	WId id();
	WId frameId();
	unsigned int damageId();

	//QWindow* window();

	QVariant property(NativeWindow::Property);
	void setProperty(NativeWindow::Property, QVariant, bool force = false);
	void setProperties(QList<NativeWindow::Property>, QList<QVariant>, bool force = false);
	void requestProperty(NativeWindow::Property, QVariant, bool force = false);
	void requestProperties(QList<NativeWindow::Property>, QList<QVariant>, bool force = false);

	QRect geometry(); //this returns the "full" geometry of the window (window + frame)

public slots:
	void toggleVisibility();
	void requestClose(); //ask the app to close the window (may/not depending on activity)
	void requestKill();	//ask the WM to kill the app associated with this window (harsh - only use if not responding)
	void requestPing();	//ask the app if it is still active (a WindowNotResponding signal will get sent out if there is no reply);

private:
	QHash <NativeWindow::Property, QVariant> hash;
	//QWindow *WIN;
	WId winid, frameid;
	QList<WId> relatedTo;
	unsigned int dmgID;

signals:
	//General Notifications
	void PropertiesChanged(QList<NativeWindow::Property>, QList<QVariant>);
	void RequestPropertiesChange(WId, QList<NativeWindow::Property>, QList<QVariant>);
	void WindowClosed(WId);
	void WindowNotResponding(WId); //will be sent out if a window does not respond to a ping request
	void VisualChanged();

	//Action Requests (not automatically emitted - typically used to ask the WM to do something)
	//Note: "WId" should be the NativeWindow id()
	void RequestClose(WId);				//Close the window
	void RequestKill(WId);					//Kill the window/app (usually from being unresponsive)
	void RequestPing(WId);				//Verify that the window is still active (such as not closing after a request
	void RequestReparent(WId, WId, QPoint); //client window, frame window, relative origin point in frame
	// System Tray Icon Embed/Unembed Requests
	//void RequestEmbed(WId, QWidget*);
	//void RequestUnEmbed(WId, QWidget*);
};

// Declare the enumerations as Qt MetaTypes
Q_DECLARE_METATYPE(NativeWindow::Type);
Q_DECLARE_METATYPE(NativeWindow::Action);
Q_DECLARE_METATYPE(NativeWindow::State);
Q_DECLARE_METATYPE(NativeWindow::Property);

#endif
