#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <EASTL/vector.h>

#include "define.h"

using namespace DirectX;
using namespace eastl;

class InstancedSphereSet
{
public:
    InstancedSphereSet(vector<InstanceData>* instanceSet) : mInstanceSet(instanceSet)
    {

    }
    ~InstancedSphereSet(void);
    void Draw(void) const;
    void UpdateInstances(void);
    InstanceData* GetInstanceHandle(int idx);    

private:
    vector<InstanceData>* mInstanceSet;
    ID3D11InputLayout* mInputLayout;
    ID3D11Buffer* mInstancesBuffer;
};

