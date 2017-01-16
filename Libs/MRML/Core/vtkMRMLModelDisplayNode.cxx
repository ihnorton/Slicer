/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLModelDisplayNode.cxx,v $
Date:      $Date: 2006/03/03 22:26:39 $
Version:   $Revision: 1.3 $

=========================================================================auto=*/
// MRML includes
#include "vtkMRMLModelDisplayNode.h"

// VTK includes
#include <vtkAlgorithmOutput.h>
#include <vtkAssignAttribute.h>
#include <vtkCommand.h>
#include <vtkObjectFactory.h>
#include <vtkPassThrough.h>
#include <vtkPointSet.h>
#include <vtkVersion.h>

//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLModelDisplayNode);

//-----------------------------------------------------------------------------
vtkMRMLModelDisplayNode::vtkMRMLModelDisplayNode()
{
  this->PassThrough = vtkPassThrough::New();
  this->AssignAttribute = vtkAssignAttribute::New();

  // the default behavior for models is to use the scalar range of the data
  // to reset the display scalar range, so use the Data flag
  this->SetScalarRangeFlag(vtkMRMLDisplayNode::UseDataScalarRange);

  // Be careful, virtualization doesn't work in constructors
  this->UpdateMeshPipeline();
}

//-----------------------------------------------------------------------------
vtkMRMLModelDisplayNode::~vtkMRMLModelDisplayNode()
{
  this->PassThrough->Delete();
  this->AssignAttribute->Delete();
}

//---------------------------------------------------------------------------
void vtkMRMLModelDisplayNode::ProcessMRMLEvents(vtkObject *caller,
                                                unsigned long event,
                                                void *callData )
{
  this->Superclass::ProcessMRMLEvents(caller, event, callData);

  if (event == vtkCommand::ModifiedEvent)
    {
    this->UpdateMeshPipeline();
    }
}

//---------------------------------------------------------------------------
void vtkMRMLModelDisplayNode
::SetInputMeshConnection(vtkAlgorithmOutput* meshConnection)
{
  if (this->GetInputMeshConnection() == meshConnection)
    {
    return;
    }
  this->SetInputToMeshPipeline(meshConnection);
  this->Modified();
}

//---------------------------------------------------------------------------
void vtkMRMLModelDisplayNode
::SetInputToMeshPipeline(vtkAlgorithmOutput* meshConnection)
{
  this->PassThrough->SetInputConnection(meshConnection);
  this->AssignAttribute->SetInputConnection(meshConnection);
}

//---------------------------------------------------------------------------
vtkPointSet* vtkMRMLModelDisplayNode::GetInputMesh()
{
  return vtkPointSet::SafeDownCast(this->AssignAttribute->GetInput());
}

//---------------------------------------------------------------------------
vtkAlgorithmOutput* vtkMRMLModelDisplayNode::GetInputMeshConnection()
{
  return this->AssignAttribute->GetNumberOfInputConnections(0) ?
    this->AssignAttribute->GetInputConnection(0,0) : 0;
}

//---------------------------------------------------------------------------
vtkPointSet* vtkMRMLModelDisplayNode::GetOutputMesh()
{
  if (!this->GetOutputMeshConnection())
    {
    return 0;
    }
  if (!this->GetInputMesh())
    {
    return 0;
    }
  return vtkPointSet::SafeDownCast(
    this->GetOutputMeshConnection()->GetProducer()->GetOutputDataObject(
      this->GetOutputMeshConnection()->GetIndex()));
}

//---------------------------------------------------------------------------
vtkAlgorithmOutput* vtkMRMLModelDisplayNode::GetOutputMeshConnection()
{
  if (this->GetActiveScalarName())
    {
    return this->AssignAttribute->GetOutputPort();
    }
  else
    {
    return this->PassThrough->GetOutputPort();
    }
}

//---------------------------------------------------------------------------
void vtkMRMLModelDisplayNode::SetActiveScalarName(const char *scalarName)
{
  if (scalarName && this->ActiveScalarName && !strcmp(scalarName, this->ActiveScalarName))
    {
    return;
    }
  if (scalarName == 0 && this->ActiveScalarName == 0)
    {
    return;
    }
  int wasModifying = this->StartModify();
  this->Superclass::SetActiveScalarName(scalarName);
  this->UpdateMeshPipeline();
  this->EndModify(wasModifying);
}

//---------------------------------------------------------------------------
void vtkMRMLModelDisplayNode::SetActiveAttributeLocation(int location)
{
  if (location == this->ActiveAttributeLocation)
    {
    return;
    }
  int wasModifying = this->StartModify();
  this->Superclass::SetActiveAttributeLocation(location);
  this->UpdateMeshPipeline();
  this->EndModify(wasModifying);
}

//---------------------------------------------------------------------------
void vtkMRMLModelDisplayNode::UpdateMeshPipeline()
{
  this->AssignAttribute->Assign(
    this->GetActiveScalarName(),
    this->GetActiveScalarName() ? vtkDataSetAttributes::SCALARS : -1,
    this->GetActiveAttributeLocation());
  if (this->GetOutputMesh())
    {
    this->GetOutputMeshConnection()->GetProducer()->Update();
    if (this->GetAutoScalarRange())
      {
      vtkDebugMacro("UpdateMeshPipeline: Auto flag is on, resetting scalar range!");
      this->SetScalarRange(this->GetOutputMesh()->GetScalarRange());
      }
    }
}
