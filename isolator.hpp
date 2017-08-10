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

#ifndef __FOO_ISOLATOR_MODULE_HPP
#define __FOO_ISOLATOR_MODULE_HPP

#include <list>

#include <mesos/mesos.hpp>
#include <mesos/resources.hpp>

#include <mesos/slave/isolator.hpp>

#include <process/future.hpp>
#include <process/owned.hpp>
#include <process/process.hpp>

#include <stout/flags.hpp>
#include <stout/option.hpp>

#include "common.hpp"

// Forward declaration.
class FooIsolatorProcess;

class FooIsolator : public mesos::slave::Isolator
{
public:
  FooIsolator(const isolator::Flags& flags);

  virtual ~FooIsolator();

  virtual process::Future<Option<mesos::slave::ContainerLaunchInfo>> prepare(
      const mesos::ContainerID& containerId,
      const mesos::slave::ContainerConfig& containerConfig);

private:
  process::Owned<FooIsolatorProcess> process;

  process::Promise<mesos::slave::ContainerLimitation> never;
};

#endif // __FOO_ISOLATOR_MODULE_HPP
