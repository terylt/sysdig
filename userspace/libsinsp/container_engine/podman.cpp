/*
Copyright (C) 2013-2019 Draios Inc dba Sysdig.

This file is part of sysdig.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/

#include "container_engine/podman.h"
#include "sinsp.h"
#include "runc.h"
using namespace libsinsp::runc;

using namespace libsinsp::container_engine;
constexpr const cgroup_layout PODMAN_CGROUP_LAYOUT[] = {
	{"/", ""}, // non-systemd docker
	{"/libpod-", ".scope"}, // systemd docker
	{nullptr, nullptr}
};

bool podman::resolve(sinsp_threadinfo *tinfo, bool query_os_for_missing_info)
{
	auto container = std::make_shared<sinsp_container_info>();
	std::string container_id;

	if(!matches_runc_cgroups(tinfo, PODMAN_CGROUP_LAYOUT, container_id))
	{
		return false;
	}
	tinfo->m_container_id = container_id;
	container->m_type = CT_PODMAN;
	container->m_id = container_id;	

	if (container_cache().should_lookup(container->m_id, CT_PODMAN))
	{
		container->m_name = container->m_id;
		container_cache().add_container(container, tinfo);
		container_cache().notify_new_container(*container);
	}
	return true;
}
