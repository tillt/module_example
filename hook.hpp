// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License

#ifndef __FOO_HOOK_MODULE_HPP
#define __FOO_HOOK_MODULE_HPP

#include <map>
#include <string>

#include <mesos/hook.hpp>
#include <mesos/mesos.hpp>

#include <process/future.hpp>
#include <process/process.hpp>

#include <stout/option.hpp>

#include "common.hpp"


class FooHook : public mesos::Hook
{
public:
  FooHook(const Flags& flags);

  virtual ~FooHook();

  process::Future<Option<mesos::DockerTaskExecutorPrepareInfo>>
    slavePreLaunchDockerTaskExecutorDecorator(
        const Option<mesos::TaskInfo>& taskInfo,
        const mesos::ExecutorInfo& executorInfo,
        const std::string& name,
        const std::string& sandboxDirectory,
        const std::string& mappedDirectory,
        const Option<std::map<std::string, std::string>>&) override;

private:
  const Flags flags_;
};

#endif // __FOO_HOOK_MODULE_HPP
