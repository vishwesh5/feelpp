/* -*- mode: c++; coding: utf-8; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; show-trailing-whitespace: t -*- vim:fenc=utf-8:ft=cpp:et:sw=4:ts=4:sts=4

 This file is part of the Feel library

 Author(s): Thibaut Metivet <thibaut.metivet@univ-grenoble-alpes.fr>
 Date: 2018-05-18

 Copyright (C) 2018 Université de Strasbourg

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3.0 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
/**
 \file levelsetspacemanager.hpp
 \author Thibaut Metivet <metivet@math.unistra.fr>
 \date 2018-05-18
 */
#ifndef _LEVELSETSPACEMANAGER_HPP
#define _LEVELSETSPACEMANAGER_HPP 1

#include <feel/feeldiscr/functionspace.hpp>

template<
    typename ConvexType, typename BasisType, typename PeriodicityType = NoPeriodicity, 
    typename BasisPnType = BasisType
    >
class LevelSetSpaceManager
{
    typedef LevelSetSpaceManager<ConvexType, BasisType, PeriodicityType, BasisPnType> self_type;
    static const uint16_type Order = BasisType::nOrder;
    typedef double value_type;

    //--------------------------------------------------------------------//
    // Mesh
    typedef ConvexType convex_type;
    static const uint16_type nDim = convex_type::nDim;
    static const uint16_type nOrderGeo = convex_type::nOrder;
    static const uint16_type nRealDim = convex_type::nRealDim;
    typedef Mesh<convex_type> mesh_type;
    typedef boost::shared_ptr<mesh_type> mesh_ptrtype;

    //--------------------------------------------------------------------//
    // Periodicity
    typedef PeriodicityType periodicity_type;

    //--------------------------------------------------------------------//
    // Default levelset spaces
    typedef BasisType basis_levelset_type;
    typedef FunctionSpace< mesh_type, bases<basis_levelset_type>, Periodicity<periodicity_type> > space_levelset_type;
    typedef boost::shared_ptr<space_levelset_type> space_levelset_ptrtype;

    typedef typename detail::ChangeBasisPolySet<Vectorial, basis_levelset_type>::type basis_vectorial_type;
    typedef FunctionSpace<mesh_type, bases<basis_vectorial_type>, Periodicity<periodicity_type> > space_vectorial_type;
    typedef boost::shared_ptr<space_vectorial_type> space_levelset_vectorial_ptrtype;

    //--------------------------------------------------------------------//
    // isoPN levelset spaces
    typedef BasisPnType basis_levelset_PN_type;
    typedef FunctionSpace< mesh_type, bases<basis_levelset_PN_type>, Periodicity<periodicity_type> > space_levelset_PN_type;
    typedef boost::shared_ptr<space_levelset_PN_type> space_levelset_PN_ptrtype;

    typedef typename detail::ChangeBasisPolySet<Vectorial, basis_levelset_PN_type>::type basis_vectorial_PN_type;
    typedef FunctionSpace<mesh_type, bases<basis_vectorial_PN_type>, Periodicity<periodicity_type> > space_vectorial_PN_type;
    typedef boost::shared_ptr<space_vectorial_PN_type> space_vectorial_PN_ptrtype;

    //--------------------------------------------------------------------//
    // Space markers P0
    typedef Lagrange<0, Scalar, Discontinuous> basis_markers_type;
    typedef FunctionSpace<mesh_type, bases<basis_markers_type>, value_type, Periodicity<NoPeriodicity> > space_markers_type;
    typedef boost::shared_ptr<space_markers_type> space_markers_ptrtype;

    //--------------------------------------------------------------------//
    // Tensor2 symmetric function space
    //typedef Lagrange<Order, Tensor2Symm> basis_tensor2symm_type;
    typedef typename detail::ChangeBasisPolySet<Tensor2Symm, basis_levelset_type>::type basis_tensor2symm_type;
    typedef FunctionSpace<mesh_type, bases<basis_tensor2symm_type>, Periodicity<periodicity_type> > space_tensor2symm_type;
    typedef boost::shared_ptr<space_tensor2symm_type> space_tensor2symm_ptrtype;
    typedef typename space_tensor2symm_type::element_type element_tensor2symm_type;
    typedef boost::shared_ptr<element_tensor2symm_type> element_tensor2symm_ptrtype;

public:
    LevelSetSpaceManager( mesh_ptrtype const& mesh );

    // Extended doftable
    bool buildExtendedDofTable() const { return M_buildExtendedDofTable; }
    void setBuildExtendedDofTable( bool buildExtDT ) { M_buildExtendedDofTable = buildExtDT; }
    // Periodicity
    void setPeriodicity( periodicity_type const& p );
    periodicity_type const& periodicity() const { return M_periodicity; }

    // Default minimal function spaces
    void createFunctionSpaceDefault();
    // Iso PN minimal function spaces
    void createFunctionSpaceIsoPN();
    
    // Tensor2Symm function space
    void createFunctionSpaceTensor2Symm();

    mesh_ptrtype const& mesh() const { return M_mesh };
    mesh_ptrtype const& meshP1isoPN() const { return M_meshP1isoPN };

private:
    std::vector<WorldComm> const& worldsComm() const { return M_worldsComm; }

private:
    //--------------------------------------------------------------------//
    // Meshes
    mesh_ptrtype M_mesh;
    mesh_ptrtype M_meshP1isoPN;
    //--------------------------------------------------------------------//
    // WorldsComm
    std::vector<WorldComm> M_worldsComm;
    // Extended doftable
    bool M_buildExtendedDofTable;
    // Periodicity
    periodicity_type M_periodicity;
    bool M_functionSpaceCreated;
    //--------------------------------------------------------------------//
    // Default function spaces
    space_levelset_ptrtype M_spaceLevelset;
    space_levelset_vectorial_ptrtype M_spaceVectorial;
    space_markers_ptrtype M_spaceMarkers;
    // PN function spaces
    space_levelset_PN_ptrtype M_spaceLevelsetPN;
    space_levelset_PN_vectorial_ptrtype M_spaceVectorialPN;
    // IsoPN function spaces
    space_levelset_ptrtype M_spaceLevelsetIsoPN;
    space_levelset_vectorial_ptrtype M_spaceVectorialIsoPN;
    // Tensor2Symm function space
    space_tensor2symm_ptrtype M_spaceTensor2Symm;

    space_levelset_ptrtype M_subspaceLevelSet;
    space_levelset_vectorial_ptrtype M_subspaceLevelSetVec;

};

#define LEVELSETSPACEMANAGER_CLASS_TEMPLATE_DECLARATIONS \
    template< typename ConvexType, typename BasisType, typename PeriodicityType, typename BasisPnType > \
        /**/
#define LEVELSETSPACEMANAGER_CLASS_TEMPLATE_TYPE \
    LevelSet<ConvexType, BasisType, PeriodicityType, BasisPnType> \
        /**/

LEVELSETSPACEMANAGER_CLASS_TEMPLATE_DECLARATIONS
LEVELSETSPACEMANAGER_CLASS_TEMPLATE_TYPE::LevelSetSpaceManager( mesh_ptrtype const& mesh )
    : M_mesh( mesh ),
      M_worldsComm( std::vector<WorldComm>(1,mesh->worldComm()) ),
      M_buildExtendedDofTable( false ),
      M_functionSpaceCreated( false )
{
    // Default spaces
    M_spaceLevelSetVec = space_vectorial_type::New( _mesh=this->mesh(), _worldscomm=this->worldsComm() );
    M_spaceMarkers = space_markers_type::New( 
            _mesh=this->mesh(), _worldscomm=this->worldsComm(),
            _extended_doftable=std::vector<bool>(1, buildSpaceMarkersExtendedDofTable)
            );
    if( M_useCauchyAugmented )
        M_spaceTensor2Symm = space_tensor2symm_type::New( _mesh=this->mesh(), _worldscomm=this->worldsComm() );

}

LEVELSETSPACEMANAGER_CLASS_TEMPLATE_DECLARATIONS
void
LEVELSETSPACEMANAGER_CLASS_TEMPLATE_TYPE::setPeriodicity( periodicity_type const& p ) 
{
    if( M_functionSpaceCreated )
        Feel::cout << "WARNING !! Setting periodicity after spaces creation ! Need to re-build them !" << std::endl;
    M_periodicity = p;
}

LEVELSETSPACEMANAGER_CLASS_TEMPLATE_DECLARATIONS
void
LEVELSETSPACEMANAGER_CLASS_TEMPLATE_TYPE::createFunctionSpaceDefault()
{
    if( !M_spaceLevelset )
    {
        std::vector<bool> extendedDT( 1, M_buildExtendedDofTable );
        M_spaceLevelset = space_levelset_type::New( 
                _mesh=this->mesh(), 
                _worldscomm=this->worldsComm(),
                _extended_doftable=extendedDT,
                _periodicity=this->periodicity()
                );
    }
    if( !M_spaceVectorial )
    {
        M_spaceVectorial = space_vectorial_type::New( 
                _mesh=this->mesh(), 
                _worldscomm=this->worldsComm(),
                _periodicity=this->periodicity()
                );
    }

    M_functionSpaceCreated = true;
}

LEVELSETSPACEMANAGER_CLASS_TEMPLATE_DECLARATIONS
void
LEVELSETSPACEMANAGER_CLASS_TEMPLATE_TYPE::createFunctionSpaceIsoPN()
{
    if( !M_spaceLevelsetPN )
    {
        M_spaceLevelsetPN = space_levelset_type::New( 
                _mesh=this->mesh(), 
                _worldscomm=this->worldsComm(),
                _periodicity=this->periodicity()
                );
    }
    if( !M_spaceVectorialPN )
    {
        M_spaceVectorialPN = space_vectorial_type::New( 
                _mesh=this->mesh(), 
                _worldscomm=this->worldsComm(),
                _periodicity=this->periodicity()
                );
    }
    if( !M_meshP1isoPN )
    {
        M_opLagrangeP1isoPN = lagrangeP1(
                _space=this->M_spaceLevelsetPN
                );
        M_meshP1isoPN = M_opLagrangeP1isoPN->mesh();
    }
    if( !M_spaceLevelsetIsoPN )
    {
        std::vector<bool> extendedDT( 1, M_buildExtendedDofTable );
        M_spaceLevelsetIsoPN = space_levelset_type::New(
                _mesh=this->meshP1isoPN(), 
                _worldscomm=this->worldsComm(),
                _extended_doftable=extendedDT,
                _periodicity=this->periodicity()
                );
    }
    if( !M_spaceVectorialIsoPN )
    {
        M_spaceVectorialIsoPN = space_vectorial_type::New( 
                _mesh=this->meshP1isoPN(),
                _worldscomm=this->worldsComm(),
                _periodicity=this->periodicity()
                );
    }

    M_functionSpaceCreated = true;
}

#endif // _LEVELSETSPACEMANAGER_HPP
