/*
 *  Copyright 2012 Andrew Okin
 * 
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "curlutils.h"

size_t CurlBlankCallback(void* buffer, size_t size, size_t nmemb, void* userp)
{
	return size * nmemb;
}

size_t CurlLambdaCallback(void* buffer, size_t size, size_t nmemb, void* userp)
{
	// Attempt to cast userp to a lambda function
	CurlLambdaCallbackFunction *lambda = (CurlLambdaCallbackFunction*)userp;
	return lambda->operator()(buffer, size * nmemb);
}
