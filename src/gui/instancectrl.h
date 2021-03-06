/*
 * Derived from the thumbnail control example by Julian Smart
 */
#pragma once

#include "wx/dynarray.h"
#include <instance.h>
#include <insticonlist.h>

#define wxINST_MULTIPLE_SELECT    0x0010
#define wxINST_SINGLE_COLUMN      0x0020

/* Flags
 */

#define wxINST_SHIFT_DOWN  0x01
#define wxINST_CTRL_DOWN   0x02
#define wxINST_ALT_DOWN    0x04

/* Defaults
 */

#define wxINST_DEFAULT_OVERALL_SIZE wxSize(-1, -1)
#define wxINST_DEFAULT_IMAGE_SIZE wxSize(32, 32)
#define wxINST_DEFAULT_SPACING 3
#define wxINST_DEFAULT_MARGIN 3
#define wxINST_DEFAULT_UNFOCUSSED_BACKGROUND wxColour(175, 175, 175)
#define wxINST_DEFAULT_FOCUSSED_BACKGROUND wxColour(140, 140, 140)
#define wxINST_DEFAULT_UNSELECTED_BACKGROUND wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)
#define wxINST_DEFAULT_TYPE_COLOUR wxColour(0, 0, 200)
#define wxINST_DEFAULT_FOCUS_RECT_COLOUR wxColour(100, 80, 80)

/*!
 * Forward declarations
 */

class wxInstanceCtrl;

/*!
 * wxInstanceItem class declaration
 */

// Drawing styles/states
#define wxINST_SELECTED    0x01
// The control is focussed
#define wxINST_FOCUSSED    0x04
// The item itself has the focus
#define wxINST_IS_FOCUS    0x08

class wxInstanceItem: public wxObject
{
	DECLARE_CLASS ( wxInstanceItem )
public:
// Constructors

	wxInstanceItem ( Instance * inst )
	{
		m_inst = inst;
		updateName();
	}

	const wxString GetName() const
	{
		return m_inst->GetName();
	}

	void updateName();

	int GetNumLines()
	{
		return text_lines;
	};
	
	/// Draw the item
	virtual bool Draw ( wxDC& dc, wxInstanceCtrl* ctrl, const wxRect& rect, int style ) ;

	/// Draw the background
	virtual bool DrawBackground ( wxDC& dc, wxInstanceCtrl* ctrl, const wxRect& rect, const wxRect& imageRect, int style, int index ) ;
	
protected:
	Instance   *m_inst;
	int        text_width;
	wxString name_wrapped;
	int        text_lines;
};

WX_DECLARE_OBJARRAY ( wxInstanceItem, wxInstanceItemArray );

/*!
 * wxInstanceCtrl class declaration
 */

class wxInstanceCtrl: public wxScrolledWindow
{
	DECLARE_CLASS ( wxInstanceCtrl )
	DECLARE_EVENT_TABLE()

public:
// Constructors

	wxInstanceCtrl( );
	wxInstanceCtrl ( wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0 );

// Operations

	/// Creation
	bool Create ( wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0 );

	/// Member initialisation
	void Init();

	/// Call Freeze to prevent refresh
	void Freeze();

	/// Call Thaw to refresh
	void Thaw();

	/// Scrolls the item into view if necessary
	void EnsureVisible ( int n );

	/// Draws the item. Normally you override function in wxInstanceItem.
	virtual bool DrawItem ( int n, wxDC& dc, const wxRect& rect, int style ) ;

	/// Draws the background for the item, including bevel
	virtual bool DrawItemBackground ( int n, wxDC& dc, const wxRect& rect, const wxRect& imageRect, int style ) ;

// Adding items

	/// Append a single item
	virtual int Append ( wxInstanceItem* item );

	/// Insert a single item
	virtual int Insert ( wxInstanceItem* item, int pos = 0 );

// Deleting items

	/// Clear all items
	virtual void Clear() ;

	/// Delete this item
	virtual void Delete ( int n ) ;

// Accessing items

	/// Get the number of items in the control
	virtual int GetCount() const
	{
		return m_items.GetCount();
	}

	/// Is the control empty?
	bool IsEmpty() const
	{
		return GetCount() == 0;
	}

	/// Get the nth item
	wxInstanceItem* GetItem ( int n );

	/// Get the overall rect of the given item
	/// If view_relative is true, rect is relative to the scroll viewport
	/// (i.e. may be negative)
	bool GetItemRect ( int item, wxRect& rect, bool view_relative = true );

	/// Get the image rect of the given item
	/// If view_relative is true, rect is relative to the scroll viewport
	/// (i.e. may be negative)
	bool GetItemRectImage ( int item, wxRect& rect, bool view_relative = true );

	/// Return the row and column given the client size
	bool GetRowCol ( int item, const wxSize& clientSize, int& row, int& col );

	/// Get the focus item, or -1 if there is none
	int GetFocusItem() const
	{
		return m_focusItem;
	}

	/// Set the focus item
	void SetFocusItem ( int item ) ;

// Selection

	/// Select or deselect an item
	void Select ( int n, bool select = true ) ;

	/// Select or deselect a range
	void SelectRange ( int from, int to, bool select = true ) ;

	/// Select all
	void SelectAll() ;

	/// Select none
	void SelectNone() ;

	/// Get the index of the single selection, if not multi-select.
	/// Returns -1 if there is no selection.
	int GetSelection() const ;

	/// Get indexes of all selections, if multi-select
	const wxArrayInt& GetSelections() const
	{
		return m_selections;
	}

	/// Returns true if the item is selected
	bool IsSelected ( int n ) const ;

	/// Clears all selections
	void ClearSelections();

// Visual properties

	/// The size of the image part
	void SetImageSize ( const wxSize& sz );
	const wxSize& GetImageSize() const
	{
		return m_ImageSize;
	}

	/// The inter-item spacing
	void SetSpacing ( int spacing )
	{
		m_spacing = spacing;
	}
	int GetSpacing() const
	{
		return m_spacing;
	}

	/// The margin between elements within the item
	void SetItemMargin ( int margin )
	{
		m_itemMargin = margin;
	}
	int GetItemMargin() const
	{
		return m_itemMargin;
	}
	
	// get height of item n
	int GetItemHeight(int n) const
	{
		wxInstanceItem & item = m_items[n];
		return m_itemMargin * 3 + m_ImageSize.y + item.GetNumLines() * m_itemTextHeight;
	}

	/// The height required for text in the item
	void SetItemTextHeight ( int h )
	{
		m_itemTextHeight = h;
	}
	int GetItemTextHeight() const
	{
		return m_itemTextHeight;
	}

// Command handlers

	void OnSelectAll ( wxCommandEvent& event );
	void OnUpdateSelectAll ( wxUpdateUIEvent& event );

// Event handlers

	/// Painting
	void OnPaint ( wxPaintEvent& event );
	void OnEraseBackground ( wxEraseEvent& event );

	/// Left-click
	void OnLeftClick ( wxMouseEvent& event );

	/// Left-double-click
	void OnLeftDClick ( wxMouseEvent& event );

	/// Middle-click
	void OnMiddleClick ( wxMouseEvent& event );

	/// Right-click
	void OnRightClick ( wxMouseEvent& event );

	/// Key press
	void OnChar ( wxKeyEvent& event );

	/// Sizing
	void OnSize ( wxSizeEvent& event );

	/// Setting/losing focus
	void OnSetFocus ( wxFocusEvent& event );
	void OnKillFocus ( wxFocusEvent& event );

// Implementation

	/// Update the row heights for layouting.
	void UpdateRows();

	/// Set up scrollbars, e.g. after a resize
	void SetupScrollbars();

	/// Calculate the outer item size based
	/// on font used for text and inner size
	void CalculateOverallItemSize();

	/// Do (de)selection
	void DoSelection ( int n, int flags );

	/// Find the item under the given point
	bool HitTest ( const wxPoint& pt, int& n );

	/// Keyboard navigation
	virtual bool Navigate ( int keyCode, int flags );

	/// Scroll to see the image (used from Navigate)
	void ScrollIntoView ( int n, int keyCode );

	/// Paint the background
	void PaintBackground ( wxDC& dc );

	/// Recreate buffer bitmap if necessary
	bool RecreateBuffer ( const wxSize& size = wxDefaultSize );

// Overrides
	wxSize DoGetBestSize() const ;

// Data members
private:

	/// The items
	wxInstanceItemArray    m_items;

	/// The selections
	wxArrayInt              m_selections;

	/// y positions where each row starts
	wxArrayInt              m_row_ys;

	/// height of each row (spacing not included)
	wxArrayInt              m_row_heights;

	/// Outer size of the item
	int                     m_itemWidth;

	/// Image size of the item
	wxSize                  m_ImageSize;

	/// The inter-item spacing
	int                     m_spacing;

	/// The margin between the image/text and the edge of the item
	int                     m_itemMargin;

	/// The height of item text in the current font
	int                     m_itemTextHeight;

	/// Allows nested Freeze/Thaw
	int                     m_freezeCount;

	/// First selection in a range
	int                     m_firstSelection;

	/// Last selection
	int                     m_lastSelection;

	/// Focus item
	int                     m_focusItem;

	/// Buffer bitmap
	wxBitmap                m_bufferBitmap;
};

/*!
 * wxInstanceCtrlEvent - the event class for wxInstanceCtrl notifications
 */

class wxInstanceCtrlEvent : public wxNotifyEvent
{
public:
	wxInstanceCtrlEvent ( wxEventType commandType = wxEVT_NULL, int winid = 0, wxPoint position = wxPoint(-1,-1) )
		: wxNotifyEvent ( commandType, winid ),
		  m_itemIndex ( -1 ), m_flags ( 0 ), m_position(position)
	{ }

	wxInstanceCtrlEvent ( const wxInstanceCtrlEvent& event )
		: wxNotifyEvent ( event ),
		  m_itemIndex ( event.m_itemIndex ), m_flags ( event.m_flags ), m_position(event.m_position)
	{ }

	int GetIndex() const
	{
		return m_itemIndex;
	}
	void SetIndex ( int n )
	{
		m_itemIndex = n;
	}

	int GetFlags() const
	{
		return m_flags;
	}
	void SetFlags ( int flags )
	{
		m_flags = flags;
	}
	
	wxPoint GetPosition ( ) const
	{
		return m_position;
	};
	void SetPosition ( wxPoint position )
	{
		m_position = position;
	};
	virtual wxEvent *Clone() const
	{
		return new wxInstanceCtrlEvent ( *this );
	}

protected:
	int           m_itemIndex;
	int           m_flags;
	wxPoint       m_position;

private:
	DECLARE_DYNAMIC_CLASS_NO_ASSIGN ( wxInstanceCtrlEvent )
};

/*!
 * wxInstanceCtrl event macros
 */

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE ( wxEVT_COMMAND_INST_ITEM_SELECTED, 2600 )
DECLARE_EVENT_TYPE ( wxEVT_COMMAND_INST_ITEM_DESELECTED, 2601 )
DECLARE_EVENT_TYPE ( wxEVT_COMMAND_INST_LEFT_CLICK, 2602 )
DECLARE_EVENT_TYPE ( wxEVT_COMMAND_INST_RIGHT_CLICK, 2603 )
DECLARE_EVENT_TYPE ( wxEVT_COMMAND_INST_MIDDLE_CLICK, 2604 )
DECLARE_EVENT_TYPE ( wxEVT_COMMAND_INST_LEFT_DCLICK, 2605 )
DECLARE_EVENT_TYPE ( wxEVT_COMMAND_INST_RETURN, 2606 )
DECLARE_EVENT_TYPE ( wxEVT_COMMAND_INST_DELETE, 2607 )
DECLARE_EVENT_TYPE ( wxEVT_COMMAND_INST_RENAME, 2608 )
END_DECLARE_EVENT_TYPES()

typedef void ( wxEvtHandler::*wxInstanceCtrlEventFunction ) ( wxInstanceCtrlEvent& );

#define EVT_INST_ITEM_SELECTED(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_INST_ITEM_SELECTED, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxInstanceCtrlEventFunction, & fn ), NULL ),
#define EVT_INST_ITEM_DESELECTED(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_INST_ITEM_DESELECTED, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxInstanceCtrlEventFunction, & fn ), NULL ),
#define EVT_INST_LEFT_CLICK(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_INST_LEFT_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxInstanceCtrlEventFunction, & fn ), NULL ),
#define EVT_INST_RIGHT_CLICK(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_INST_RIGHT_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxInstanceCtrlEventFunction, & fn ), NULL ),
#define EVT_INST_MIDDLE_CLICK(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_INST_MIDDLE_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxInstanceCtrlEventFunction, & fn ), NULL ),
#define EVT_INST_LEFT_DCLICK(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_INST_LEFT_DCLICK, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxInstanceCtrlEventFunction, & fn ), NULL ),
#define EVT_INST_RETURN(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_INST_RETURN, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxInstanceCtrlEventFunction, & fn ), NULL ),
#define EVT_INST_DELETE(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_INST_DELETE, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxInstanceCtrlEventFunction, & fn ), NULL ),
#define EVT_INST_RENAME(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_INST_RENAME, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxInstanceCtrlEventFunction, & fn ), NULL ),

