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

#include "isolator.hpp"

#include <map>
#include <string>

#include <mesos/mesos.hpp>
#include <mesos/module.hpp>
#include <mesos/resources.hpp>

#include <mesos/module/isolator.hpp>

#include <mesos/slave/isolator.hpp>

#include <process/clock.hpp>
#include <process/future.hpp>
#include <process/owned.hpp>
#include <process/process.hpp>

#include <stout/flags.hpp>
#include <stout/hashset.hpp>
#include <stout/nothing.hpp>
#include <stout/option.hpp>
#include <stout/protobuf.hpp>
#include <stout/try.hpp>

#include "common.hpp"


using namespace mesos;
using namespace process;

using std::string;

using mesos::slave::ContainerLaunchInfo;
using mesos::slave::Isolator;

class FooIsolatorProcess
  : public process::Process<FooIsolatorProcess>
{
public:
  FooIsolatorProcess(const isolator::Flags& flags) : flags_(flags) {}

  virtual Future<Option<ContainerLaunchInfo>> prepare(
      const ContainerID& containerId,
      const slave::ContainerConfig& containerConfig)
  {
    Environment executorEnvironment;
    Environment taskEnvironment;

    ContainerLaunchInfo launchInfo;

    launchInfo.mutable_environment()->CopyFrom(executorEnvironment.get());
    launchInfo.mutable_task_environment()->CopyFrom(taskEnvironment.get());

    return launchInfo;
  }

private:
  const isolator::Flags flags_;
};


FooIsolator::FooIsolator(const isolator::Flags& flags)
  : process(new FooIsolatorProcess(flags))
{
  spawn(process.get());
}


FooIsolator::~FooIsolator()
{
  terminate(process.get());
  wait(process.get());
}


Future<Option<slave::ContainerLaunchInfo>> FooIsolator::prepare(
    const ContainerID& containerId,
    const slave::ContainerConfig& containerConfig)
{
  return dispatch(process.get(),
                  &FooIsolatorProcess::prepare,
                  containerId,
                  containerConfig);
}


mesos::modules::Module<Isolator>
org_apache_mesos_FooIsolator(
    MESOS_MODULE_API_VERSION,
    MESOS_VERSION,
    "ACME",
    "modules@acme.com",
    "Foo isolator module.",
    nullptr,
    [](const Parameters& parameters) -> Isolator* {
      // Convert `parameters` into a map.
      map<string, string> values;
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

      return new FooIsolator(flags);
    });
