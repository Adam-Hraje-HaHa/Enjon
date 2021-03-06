#ifndef ENJON_H
#define ENJON_H

/////////////////////
//  ENJON INCLUDES //
/////////////////////

#include "System/Config.h"
#include "System/Types.h"
#include "System/Containers.h"

#include "Utils/Timing.h"
#include "Utils/FileUtils.h"
#include "Utils/Errors.h"

#include "Math/Maths.h"
#include "Math/Random.h" 
#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Vec4.h"
#include "Math/Quaternion.h"
#include "Math/Transform.h"

#include "Graphics/Window.h"
#include "Graphics/Vertex.h"
#include "Graphics/Window.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/Font.h"
#include "Graphics/FontManager.h"
#include "Graphics/GraphicsSubsystem.h"
#include "Graphics/GBuffer.h"
#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/GraphicsScene.h"
#include "Graphics/ShaderGraph.h"
#include "Graphics/Renderable.h"
#include "Graphics/DirectionalLight.h"
#include "Graphics/SpotLight.h"
#include "Graphics/PointLight.h"
#include "Graphics/QuadBatch.h"
#include "Graphics/GraphicsSubsystem.h"
#include "Graphics/Skeleton.h"

#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

#include "GUI/Signal.h"
#include "GUI/UIAsset.h"

#include "IO/InputManager.h"

#include "ImGui/ImGuiManager.h"

#include "Asset/AssetManager.h"

#include "Physics/PhysicsSubsystem.h" 

#include "Entity/EntityManager.h"
#include "Entity/Entity.h"
#include "Entity/Component.h"

#include "Entity/Components/PointLightComponent.h"
#include "Entity/Components/StaticMeshComponent.h" 
#include "Entity/Components/BoxComponent.h" 
#include "Entity/Components/RigidBodyComponent.h" 
#include "Entity/Components/CameraComponent.h"
#include "Entity/Components/SkeletalMeshComponent.h"
#include "Entity/Components/SkeletalAnimationComponent.h"

#include "Application.h"
#include "TypeCatalog.h"
#include "Engine.h"
#include "Defines.h"

#include "Serialize/AssetArchiver.h"
#include "Serialize/ObjectArchiver.h"
#include "Serialize/ByteBuffer.h"
#include "Serialize/EntityArchiver.h"
#include "Serialize/UUID.h"

#include "SubsystemCatalog.h"
#include "Engine.h"

#endif