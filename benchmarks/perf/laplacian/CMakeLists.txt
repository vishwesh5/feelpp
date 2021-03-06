
###  CMakeLists.txt ---

#  Author(s): Christophe Prud'homme <christophe.prudhomme@feelpp.org>
#       Date: 2010-06-12
#
#  Copyright (C) 2010 Universite Joseph Fourier (Grenoble I)
#
# Distributed under the GPL(GNU Public License):
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
#
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
add_custom_target(gen_feel_perf_laplacian ALL )
#foreach(N 2 3 4 5)
#foreach(S "Simplex" "Hypercube")
foreach(S "Simplex")
foreach(P "Lagrange")
foreach(D 2 3 )
  foreach(N 1 2 3 4 5 )
  #foreach(N 1 2)
    set( LAG "// this file is automatically generated
#include <laplacian.hpp>
namespace Feel {
  template class Laplacian<${D}, ${P}<${N}>, ${S}>\\;
}
")
    OVERWITE_IF_DIFFERENT(gen_feel_perf_laplacian "feel_perf_laplacian_${S}_${D}D_${P}_${N}.cpp" "${LAG}" laplacian.hpp)
    set(lagrange "${CMAKE_CURRENT_BINARY_DIR}/feel_perf_laplacian_${S}_${D}D_${P}_${N}.cpp;${lagrange}")
  endforeach()
endforeach()
endforeach()
endforeach()

foreach(S "Hypercube" "Simplex")
foreach(P "CrouzeixRaviart")
foreach(D 2)
  foreach(N 1 )
    set( LAG "// this file is automatically generated
#include <laplacian.hpp>
namespace Feel {
  template class Laplacian<${D}, ${P}<${N}>, ${S}>\\;
}
")
    OVERWITE_IF_DIFFERENT(gen_feel_perf_laplacian "feel_perf_laplacian_${S}_${D}D_${P}_${N}.cpp" "${LAG}" laplacian.hpp)
    set(cr "${CMAKE_CURRENT_BINARY_DIR}/feel_perf_laplacian_${S}_${D}D_${P}_${N}.cpp;${cr}")
  endforeach()
endforeach()
endforeach()
endforeach()

foreach(S "Hypercube")
foreach(P "CrouzeixRaviart")
foreach(D 2)
  foreach(N 1 )
    set( LAG "// this file is automatically generated
#include <laplacianv.hpp>
namespace Feel {
  template class LaplacianV<${D}, ${P}<${N},Vectorial>, ${S}>\\;
}
")
    OVERWITE_IF_DIFFERENT(gen_feel_perf_laplacian "feel_perf_laplacian_${S}_${D}D_${P}_Vectorial_${N}.cpp" "${LAG}" laplacianv.hpp)
    set(cr "${CMAKE_CURRENT_BINARY_DIR}/feel_perf_laplacian_${S}_${D}D_${P}_Vectorial_${N}.cpp;${cr}")
  endforeach()
endforeach()
endforeach()
endforeach()

feelpp_add_application( perf_laplacian
                        SRCS ${lagrange} bench.cpp
                        DEPS gen_feel_perf_laplacian
                        CFG laplacian.cfg laplacian-ellipsoid.cfg )
#add_executable(feel_perf_laplacian  ${lagrange} bench.cpp)
#add_executable(feel_perf_laplacian  ${lagrange} ${cr} bench.cpp)
#add_executable(feel_perf_laplacian ${CMAKE_CURRENT_BINARY_DIR}/feel_perf_laplacian_Hypercube_2D_CrouzeixRaviart_1.cpp  bench.cpp)
#add_dependencies(feel_perf_laplacian gen_feel_perf_laplacian)
#target_link_libraries(feel_perf_laplacian ${FEELPP_LIBRARIES})


#configure_file(laplacian.cfg laplacian.cfg)
