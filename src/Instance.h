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

#pragma once
#include "includes.h"

bool IsValidInstance(wxFileName rootDir);

class Instance
{
public:
	static Instance *LoadInstance(wxFileName rootDir);
	Instance(wxFileName rootDir, wxString name);
	~Instance(void);

	bool Save();
	bool Load(bool loadDefaults = false);

	wxFileName GetRootDir();
	wxFileName GetConfigPath();

	wxString GetName();
	void SetName(wxString name);

	wxString GetIconKey();
	void SetIconKey(wxString iconKey);

protected:
	wxFileName rootDir;

	wxString name;
	wxString iconKey;
	wxString notes;
	bool needsRebuild;
	bool askUpdate;
};
