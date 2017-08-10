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

#include "hook.hpp"

#include <map>
#include <string>
#include <vector>

#include <mesos/hook.hpp>
#include <mesos/mesos.hpp>
#include <mesos/module.hpp>

#include <mesos/module/hook.hpp>

#include <process/future.hpp>

#include <stout/error.hpp>
#include <stout/foreach.hpp>
#include <stout/option.hpp>
#include <stout/protobuf.hpp>
#include <stout/result.hpp>
#include <stout/strings.hpp>

#include "common.hpp"


using namespace mesos;
using namespace process;

using std::string;

FooHook::FooHook(const Flags& flags) : flags_(flags) {}


FooHook::~FooHook() {}


// This hook is modifying both the executor as well as the task
// specific environments.
// `containerWorkDirectory` is the sandbox path on the host machine filesystem.
// `mappedSandboxDirectory` is the sandbox path in the container's filesystem.

Future<Option<DockerTaskExecutorPrepareInfo>>
FooHook::slavePreLaunchDockerTaskExecutorDecorator(
    const Option<TaskInfo>&,
    const ExecutorInfo&,
    const string&,
    const string& containerWorkDirectory,
    const string& mappedSandboxDirectory,
    const Option<map<string, string>>&)
{
  Environment executorEnvironment;
  Environment taskEnvironment;

  DockerTaskExecutorPrepareInfo decoratorInfo;
  decoratorInfo.mutable_executorenvironment()->CopyFrom(
      executorEnvironment;
  decoratorInfo.mutable_taskenvironment()->CopyFrom(
      taskEnvironment);

  return decoratorInfo;
}


mesos::modules::Module<Hook> org_apache_mesos_FooHook(
    MESOS_MODULE_API_VERSION,
    MESOS_VERSION,
    "ACME",
    "modules@acme.com",
    "Foo hook module.",
    nullptr,
    [](const Parameters& parameters) -> Hook* {
      // Convert `parameters` into a map.
      std::map<std::string, std::string> values;
      foreach (const Parameter& parameter, parameters.parameter()) {
        values[parameter.key()] = parameter.value();
      }

      Flags flags;

      // Load and validate flags from environment and map.
      Try<flags::Warnings> load = flags.load(values, false, "ACME_");

      if (load.isError()) {
        LOG(ERROR) << "Failed to parse parameters: " << load.error();
        return nullptr;
      }

      foreach (const flags::Warning& warning, load->warnings) {
        LOG(WARNING) << warning.message;
      }

      return new FooHook(flags);
    });
