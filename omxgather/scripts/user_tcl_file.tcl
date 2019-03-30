# Copyright (C) 2019, Xilinx Inc - All rights reserved
#
# Licensed under the Apache License, Version 2.0 (the "License"). You may
# not use this file except in compliance with the License. A copy of the
# License is located at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations
# under the License.


set ::env(HMSS_RA_PORT_LIST) [get_bd_intf_pins -of [get_bd_cells omx*] -filter {mode=="Master" && name !~ "*inbuf*" && name !~ "*outbuf*"}]
validate_bd_design -force
