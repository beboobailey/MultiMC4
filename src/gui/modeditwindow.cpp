// 
//  Copyright 2012 Andrew Okin
// 
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
// 
//        http://www.apache.org/licenses/LICENSE-2.0
// 
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//

#include "modeditwindow.h"
#include <wx/notebook.h>
#include <wx/clipbrd.h>
#include <apputils.h>

#include "exportinstwizard.h"

#include "mainwindow.h"

ModEditWindow::ModEditWindow(MainWindow *parent, Instance *inst)
	: wxFrame(parent, -1, _("Edit Mods"), wxDefaultPosition, wxSize(500, 400))
{
	m_mainWin = parent;
	wxPanel *mainPanel = new wxPanel(this);

	SetTitle(wxString::Format(_("Edit Mods for %s"), inst->GetName().c_str()));
	
	m_inst = inst;
	
	wxBoxSizer *mainBox = new wxBoxSizer(wxVERTICAL);
	mainPanel->SetSizer(mainBox);
	
	wxNotebook *modEditNotebook = new wxNotebook(mainPanel, -1);
	mainBox->Add(modEditNotebook, wxSizerFlags(1).Expand().Border(wxALL, 8));
	
	wxPanel *jarModPanel = new wxPanel(modEditNotebook, -1);
	wxBoxSizer *jarModSizer = new wxBoxSizer(wxHORIZONTAL);
	jarModPanel->SetSizer(jarModSizer);
	modEditNotebook->AddPage(jarModPanel, _("Jar Mods"), true);
	
	jarModList = new JarModListCtrl(jarModPanel, ID_JAR_MOD_LIST, inst);
	jarModList->InsertColumn(0, _("Mod Name"));
	jarModList->InsertColumn(1, _("Mod Version"), wxLIST_FORMAT_RIGHT);
	jarModList->SetDropTarget(new JarModsDropTarget(jarModList, inst));
	jarModSizer->Add(jarModList, wxSizerFlags(1).Expand().Border(wxALL, 8));
	
	wxBoxSizer *jarListBtnBox = new wxBoxSizer(wxVERTICAL);
	jarModSizer->Add(jarListBtnBox, wxSizerFlags(0).Border(wxTOP | wxBOTTOM, 4));
	
	wxButton *addJarModBtn = new wxButton(jarModPanel, ID_ADD_JAR_MOD, _("&Add"));
	jarListBtnBox->Add(addJarModBtn, wxSizerFlags(0).Border(wxTOP | wxBOTTOM | wxRIGHT, 4));
	
	delJarModBtn = new wxButton(jarModPanel, ID_DEL_JAR_MOD, _("&Remove"));
	jarListBtnBox->Add(delJarModBtn, wxSizerFlags(0).Border(wxTOP | wxBOTTOM | wxRIGHT, 4));
	
	jarModUpBtn = new wxButton(jarModPanel, ID_MOVE_JAR_MOD_UP, _("Move &Up"));
	jarListBtnBox->Add(jarModUpBtn, wxSizerFlags(0).Border(wxTOP | wxBOTTOM | wxRIGHT, 4).Align(wxALIGN_BOTTOM));
	
	jarModDownBtn = new wxButton(jarModPanel, ID_MOVE_JAR_MOD_DOWN, _("Move &Down"));
	jarListBtnBox->Add(jarModDownBtn, wxSizerFlags(0).Border(wxTOP | wxBOTTOM | wxRIGHT, 4).Align(wxALIGN_BOTTOM));
	
	
	wxPanel *mlModPanel = new wxPanel(modEditNotebook, -1);
	wxBoxSizer *mlModSizer = new wxBoxSizer(wxHORIZONTAL);
	mlModPanel->SetSizer(mlModSizer);
	modEditNotebook->AddPage(mlModPanel, _("Mods Folder"));
	
	mlModList = new MLModListCtrl(mlModPanel, ID_ML_MOD_LIST, inst);
	mlModList->InsertColumn(0, _("Mod Name"));
	mlModList->InsertColumn(1, _("Mod Version"), wxLIST_FORMAT_RIGHT);
	mlModList->SetDropTarget(new MLModsDropTarget(mlModList, inst));
	mlModSizer->Add(mlModList, wxSizerFlags(1).Expand().Border(wxALL, 8));
	
	wxBoxSizer *mlModListBtnBox = new wxBoxSizer(wxVERTICAL);
	mlModSizer->Add(mlModListBtnBox, wxSizerFlags(0).Border(wxTOP | wxBOTTOM, 4));
	
	wxButton *addMLModBtn = new wxButton(mlModPanel, ID_ADD_ML_MOD, _("&Add"));
	mlModListBtnBox->Add(addMLModBtn, wxSizerFlags(0).Border(wxTOP | wxBOTTOM | wxRIGHT, 4));
	
	wxButton *delMLModBtn = new wxButton(mlModPanel, ID_DEL_ML_MOD, _("&Remove"));
	mlModListBtnBox->Add(delMLModBtn, wxSizerFlags(0).Border(wxTOP | wxBOTTOM | wxRIGHT, 4));
	
	
	wxBoxSizer *btnBox = new wxBoxSizer(wxHORIZONTAL);
	mainBox->Add(btnBox, 0, wxALIGN_RIGHT | wxBOTTOM | wxRIGHT | wxLEFT, 8);

	wxButton *btnExport = new wxButton(mainPanel, ID_EXPORT, _("&Export"));
	btnBox->Add(btnExport, wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 4));

	wxButton *btnClose = new wxButton(mainPanel, wxID_CLOSE, _("&Close"));
	btnBox->Add(btnClose, wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 4));
	
	CenterOnParent();
	
	LoadJarMods();
	LoadMLMods();
}

void ModEditWindow::LoadJarMods()
{
	jarModList->UpdateItems();
}

void ModEditWindow::ModListCtrl::UpdateItems()
{
	SetItemCount(GetModList()->size());
	Refresh();
	Update();
}

void ModEditWindow::LoadMLMods()
{
	mlModList->UpdateItems();
}

ModEditWindow::ModListCtrl::ModListCtrl(wxWindow *parent, int id, Instance *inst)
	: wxListCtrl(parent, id, wxDefaultPosition, wxDefaultSize, 
				 wxLC_REPORT | wxLC_VIRTUAL | wxLC_VRULES)
{
	m_inst = inst;
	m_insMarkIndex = -1;

	const int accelCount = 3;
	wxAcceleratorEntry entries[accelCount];
	entries[0].Set(wxACCEL_NORMAL,	WXK_DELETE,	wxID_DELETE);
	entries[1].Set(wxACCEL_CTRL,	(int) 'C',	wxID_COPY);
	entries[2].Set(wxACCEL_CTRL,	(int) 'V',	wxID_PASTE);
	wxAcceleratorTable accel(accelCount, entries);
	SetAcceleratorTable(accel);
}

wxString ModEditWindow::ModListCtrl::OnGetItemText(long int item, long int column) const
{
	switch (column)
	{
	case 0:
		return (*GetModList())[item].GetName();
	case 1:
		return (*GetModList())[item].GetModVersion();
	default:
		return wxEmptyString;
	}
}

ModList *ModEditWindow::ModListCtrl::GetModList() const
{
	if (GetId() == ID_ML_MOD_LIST)
		return m_inst->GetMLModList();
	else
		return m_inst->GetModList();
}

void ModEditWindow::UpdateColSizes()
{
	const int versionColumnWidth = 100;
	
	int width = jarModList->GetClientSize().GetWidth();
	
	jarModList->SetColumnWidth(0, width - versionColumnWidth);
	jarModList->SetColumnWidth(1, versionColumnWidth);
	
	mlModList->SetColumnWidth(0, width - versionColumnWidth);
	mlModList->SetColumnWidth(1, versionColumnWidth);
}

bool ModEditWindow::Show(bool show)
{
	bool response = wxFrame::Show(show);
	UpdateColSizes();
	return response;
}

ModEditWindow::JarModsDropTarget::JarModsDropTarget(ModEditWindow::ModListCtrl *owner, Instance *inst)
{
	m_owner = owner;
	m_inst = inst;
}

wxDragResult ModEditWindow::JarModsDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
{
	int flags = wxLIST_HITTEST_BELOW;
	long index = 0;
	
	if (m_owner->GetItemCount() > 0)
	{
		index = m_owner->HitTest(wxPoint(x, y), flags);
	}
	
	def = wxDragCopy;
	if (index != wxNOT_FOUND)
	{
		m_owner->SetInsertMark(index);
	}
	else
	{
		m_owner->SetInsertMark(m_owner->GetItemCount());
	}
	
	return def;
}

void ModEditWindow::JarModsDropTarget::OnLeave()
{
	m_owner->SetInsertMark(-1);
}

bool ModEditWindow::JarModsDropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString &filenames)
{
	int flags = wxLIST_HITTEST_ONITEM;
	
	long index = 0;
	
	if (m_owner->GetItemCount() > 0)
	{
		index = m_owner->HitTest(wxPoint(x, y), flags);
	}
	
	m_owner->SetInsertMark(-1);
	
	for (wxArrayString::const_iterator iter = filenames.begin(); iter != filenames.end(); iter++)
	{
		wxFileName modFileName(*iter);
		m_inst->GetModList()->InsertMod(index, modFileName.GetFullPath());
		m_owner->UpdateItems();
	}

	return true;
}

void ModEditWindow::ModListCtrl::OnCopyMod(wxCommandEvent &event)
{
	CopyMod();
}

void ModEditWindow::ModListCtrl::OnPasteMod(wxCommandEvent &event)
{
	PasteMod();
}

void ModEditWindow::ModListCtrl::OnDeleteMod(wxCommandEvent &event)
{
	DeleteMod();
}


void ModEditWindow::JarModListCtrl::PasteMod()
{
	wxFileDataObject data;

	// Get data from the clipboard.
	if (wxTheClipboard->Open())
	{
		if (wxTheClipboard->IsSupported(wxDF_FILENAME))
		{
			wxTheClipboard->GetData(data);
		}
		else
		{
			wxTheClipboard->Close();
			return;
		}
		wxTheClipboard->Close();
	}

	// Determine where to insert the files.
	int insertPoint = m_inst->GetModList()->size();
	if (GetSelectedItems().Count() > 0)
		insertPoint = GetSelectedItems()[0];

	// Add the given mods.
	wxArrayString filenames = data.GetFilenames();
	for (wxArrayString::iterator iter = filenames.begin(); iter != filenames.end(); ++iter)
	{
		m_inst->GetModList()->InsertMod(insertPoint, *iter);
	}
	UpdateItems();
}

void ModEditWindow::JarModListCtrl::CopyMod()
{
	ModList *mods = m_inst->GetModList();
	wxFileDataObject *modFileObj = new wxFileDataObject;

	wxArrayInt indices = GetSelectedItems();
	for (wxArrayInt::const_iterator iter = indices.begin(); iter != indices.end(); ++iter)
	{
		wxFileName modFile = mods->at(*iter).GetFileName();
		modFile.MakeAbsolute();
		modFileObj->AddFile(modFile.GetFullPath());
	}

	if (wxTheClipboard->Open())
	{
		wxTheClipboard->SetData(modFileObj);
		wxTheClipboard->Close();
	}
}

void ModEditWindow::JarModListCtrl::DeleteMod()
{
	if (GetItemCount() <= 0)
		return;
	
	wxArrayInt indices = GetSelectedItems();
	for (int i = indices.GetCount() -1; i >= 0; i--)
	{
		m_inst->GetModList()->DeleteMod(indices[i]);
	}
	UpdateItems();
}


void ModEditWindow::MLModListCtrl::PasteMod()
{
	wxFileDataObject data;

	// Get data from the clipboard.
	if (wxTheClipboard->Open())
	{
		if (wxTheClipboard->IsSupported(wxDF_FILENAME))
		{
			wxTheClipboard->GetData(data);
		}
		else
		{
			wxTheClipboard->Close();
			return;
		}
		wxTheClipboard->Close();
	}

	// Add the given mods.
	wxArrayString filenames = data.GetFilenames();
	for (wxArrayString::iterator iter = filenames.begin(); iter != filenames.end(); ++iter)
	{
		wxFileName file(*iter);
		wxCopyFile(file.GetFullPath(), Path::Combine(m_inst->GetMLModsDir(), file.GetFullName()));
	}
	//FIXME: this looks like lazy code. it can be done better.
	auto mllist = m_inst->GetMLModList();
	mllist->UpdateModList();
	UpdateItems();
}

void ModEditWindow::MLModListCtrl::CopyMod()
{
	ModList *mods = m_inst->GetMLModList();
	wxFileDataObject *modFileObj = new wxFileDataObject;

	wxArrayInt indices = GetSelectedItems();
	for (wxArrayInt::const_iterator iter = indices.begin(); iter != indices.end(); ++iter)
	{
		wxFileName modFile = mods->at(*iter).GetFileName();
		modFile.MakeAbsolute();
		modFileObj->AddFile(modFile.GetFullPath());
	}

	if (wxTheClipboard->Open())
	{
		wxTheClipboard->SetData(modFileObj);
		wxTheClipboard->Close();
	}
}

void ModEditWindow::MLModListCtrl::DeleteMod()
{
	if (GetItemCount() <= 0)
		return;
	
	wxArrayInt indices;
	long item = -1;
	while (true)
	{
		item = GetNextItem(item, wxLIST_NEXT_ALL, 
									  wxLIST_STATE_SELECTED);
		
		if (item == -1)
			break;
		
		indices.Add(item);
	}
	
	//FIXME: this looks like lazy code. it can be done better.
	for (int i = indices.GetCount() -1; i >= 0; i--)
	{
		m_inst->GetMLModList()->DeleteMod(indices[i]);
	}
	auto mllist = m_inst->GetMLModList();
	mllist->UpdateModList();
	UpdateItems();
}

void ModEditWindow::ModListCtrl::SetInsertMark(const int index)
{
	m_insMarkIndex = index;
	Refresh();
	Update();
	DrawInsertMark(index);
}

void ModEditWindow::ModListCtrl::DrawInsertMark(int index)
{
	if (index < 0)
		return;
#ifdef WIN32
	wxWindow *listMainWin = (wxWindow*)this;
#else
	wxWindow *listMainWin = (wxWindow*)this->m_mainWin;
#endif
	
#ifdef WIN32
	int offsetY = 3;
#else
	int offsetY = -listMainWin->GetPosition().y;
#endif

	int lineY = 0;
	wxRect itemRect;
	this->GetItemRect(index, itemRect);
	lineY = itemRect.GetTop() + offsetY;
	
	if (index == GetItemCount() && GetItemCount() > 0)
	{
		this->GetItemRect(index - 1, itemRect);
		lineY = itemRect.GetBottom() + offsetY;
	}
	
	wxWindowDC dc(listMainWin);
	dc.SetPen(wxPen(wxColour(_("white")), 2, wxSOLID));
	
	const wxBrush *brush = wxTRANSPARENT_BRUSH;
	dc.SetBrush(*brush);
	dc.SetLogicalFunction(wxXOR);
	dc.DrawLine(0, lineY, this->GetClientSize().GetWidth(), lineY);
}

ModEditWindow::MLModsDropTarget::MLModsDropTarget(ModEditWindow::ModListCtrl *owner, Instance *inst)
{
	m_owner = owner;
	m_inst = inst;
}

wxDragResult ModEditWindow::MLModsDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
{
	return wxDragCopy;
}

bool ModEditWindow::MLModsDropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString &filenames)
{
	for (wxArrayString::const_iterator iter = filenames.begin(); iter != filenames.end(); ++iter)
	{
		wxFileName dest(Path::Combine(m_inst->GetMLModsDir().GetFullPath(), *iter));
		wxCopyFile(*iter, dest.GetFullPath());
	}
	
	auto mllist = m_inst->GetMLModList();
	mllist->UpdateModList();
	m_owner->UpdateItems();

	return true;
}

void ModEditWindow::OnAddJarMod(wxCommandEvent &event)
{
	wxFileDialog *addModDialog = new wxFileDialog(this, _("Choose a file to add."), 
		wxGetCwd(), wxEmptyString);
	if (addModDialog->ShowModal() == wxID_OK)
	{
		wxFileName file(addModDialog->GetPath());
		m_inst->GetModList()->InsertMod(m_inst->GetModList()->size(), file.GetFullPath());
		jarModList->UpdateItems();
	}
}

void ModEditWindow::OnDeleteJarMod(wxCommandEvent &event)
{
	jarModList->DeleteMod();
}

void ModEditWindow::OnMoveJarModUp(wxCommandEvent &event)
{
	if (jarModList->GetItemCount() <= 0)
		return;
	
	wxArrayInt indices = jarModList->GetSelectedItems();
	ModList *mods= m_inst->GetModList();
	for (size_t i = 0; i < indices.GetCount(); ++i)
	{
		if (indices[i] == 0)
			continue;
		
		Mod mod = mods->at(indices[i]);
		mods->erase(mods->begin() + indices[i]);
		mods->insert(mods->begin() + indices[i] - 1, mod);
		
		jarModList->SetItemState(indices[i], 0, wxLIST_STATE_SELECTED);
		jarModList->SetItemState(indices[i] - 1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	}
	
	jarModList->UpdateItems();
}

void ModEditWindow::OnMoveJarModDown(wxCommandEvent &event)
{
	if (jarModList->GetItemCount() <= 0)
		return;
	
	wxArrayInt indices = jarModList->GetSelectedItems();
	ModList *mods= m_inst->GetModList();
	for (size_t i = indices.GetCount(); i > 0;)
	{
		--i; // Fixes faulty comparison i>=0 on unsigned i above
		if ((size_t)indices[i] + 1 >= mods->size())
			continue;
		
		Mod mod = mods->at(indices[i]);
		mods->erase(mods->begin() + indices[i]);
		mods->insert(mods->begin() + indices[i] + 1, mod);
		
		jarModList->SetItemState(indices[i], 0, wxLIST_STATE_SELECTED);
		jarModList->SetItemState(indices[i] + 1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	}
}

void ModEditWindow::OnJarModSelChanged(wxListEvent &event)
{
	int selCount = jarModList->GetSelectedItemCount();
	delJarModBtn->Enable(selCount > 0);
	jarModUpBtn->Enable(selCount > 0);
	jarModDownBtn->Enable(selCount > 0);
}


void ModEditWindow::OnAddMLMod(wxCommandEvent &event)
{
	wxFileDialog *addModDialog = new wxFileDialog(this, _("Choose a file to add."), 
												  wxGetCwd(), wxEmptyString);
	if (addModDialog->ShowModal() == wxID_OK)
	{
		wxFileName file(addModDialog->GetPath());
		//wxCopyFile(file.GetFullPath(), Path::Combine(m_inst->GetMLModsDir(), file.GetFullName()));
		m_inst->GetMLModList()->InsertMod(0, file.GetFullPath());
		mlModList->UpdateItems();
	}
}

void ModEditWindow::OnDeleteMLMod(wxCommandEvent &event)
{
	mlModList->DeleteMod();
}

wxArrayInt ModEditWindow::ModListCtrl::GetSelectedItems()
{
	wxArrayInt indices;
	long item = -1;
	while (true)
	{
		item = GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
		
		if (item == -1)
			break;
		
		indices.Add(item);
	}
	return indices;
}

void ModEditWindow::OnDragJarMod(wxListEvent &event)
{
	ModList *mods = m_inst->GetModList();
	wxFileDataObject modFileObj;
	
	wxArrayInt indices = jarModList->GetSelectedItems();
	for (wxArrayInt::const_iterator iter = indices.begin(); iter != indices.end(); ++iter)
	{
		wxFileName modFile = mods->at(*iter).GetFileName();
		modFile.MakeAbsolute();
		modFileObj.AddFile(modFile.GetFullPath());
	}
	
	wxDropSource modsDropSource(modFileObj, jarModList);
	modsDropSource.DoDragDrop(wxDrag_CopyOnly);
}

void ModEditWindow::OnExportClicked(wxCommandEvent& event)
{
	ExportInstWizard *exportWizard = new ExportInstWizard(this, this->m_inst);
	exportWizard->Start();

	wxString fileName;

	wxFileDialog *chooseFileDlg = new wxFileDialog(this, _("Save..."), 
		wxGetCwd(), wxEmptyString, _("*.zip"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (chooseFileDlg->ShowModal() == wxID_OK)
	{
		fileName = chooseFileDlg->GetPath();
	}
	else
	{
		return;
	}

	wxString packName = exportWizard->GetPackName();
	wxString packNotes = exportWizard->GetPackNotes();

	wxArrayString includedConfigs;
	exportWizard->GetIncludedConfigs(&includedConfigs);

	exportWizard->Destroy();

	m_mainWin->BuildConfPack(m_inst, packName, packNotes, fileName, includedConfigs);
}

void ModEditWindow::OnCloseClicked(wxCommandEvent &event)
{
	Close();
}


BEGIN_EVENT_TABLE(ModEditWindow, wxFrame)
	EVT_BUTTON(ID_ADD_JAR_MOD, ModEditWindow::OnAddJarMod)
	EVT_BUTTON(ID_DEL_JAR_MOD, ModEditWindow::OnDeleteJarMod)
	EVT_BUTTON(ID_MOVE_JAR_MOD_UP, ModEditWindow::OnMoveJarModUp)
	EVT_BUTTON(ID_MOVE_JAR_MOD_DOWN, ModEditWindow::OnMoveJarModDown)
	
	EVT_BUTTON(ID_ADD_ML_MOD, ModEditWindow::OnAddMLMod)
	EVT_BUTTON(ID_DEL_ML_MOD, ModEditWindow::OnDeleteMLMod)
	
	EVT_LIST_ITEM_SELECTED(ID_JAR_MOD_LIST, ModEditWindow::OnJarModSelChanged)
	EVT_LIST_ITEM_DESELECTED(ID_JAR_MOD_LIST, ModEditWindow::OnJarModSelChanged)
	
	EVT_LIST_BEGIN_DRAG(ID_JAR_MOD_LIST, ModEditWindow::OnDragJarMod)
	
	EVT_BUTTON(ID_EXPORT, ModEditWindow::OnExportClicked)
	EVT_BUTTON(wxID_CLOSE, ModEditWindow::OnCloseClicked)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(ModEditWindow::ModListCtrl, wxListCtrl)
	EVT_MENU(wxID_COPY, ModEditWindow::ModListCtrl::OnCopyMod)
	EVT_MENU(wxID_PASTE, ModEditWindow::ModListCtrl::OnPasteMod)
	EVT_MENU(wxID_DELETE, ModEditWindow::ModListCtrl::OnDeleteMod)
END_EVENT_TABLE()
