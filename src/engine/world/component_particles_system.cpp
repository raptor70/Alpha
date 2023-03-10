#include "component_particles_system.h"

#include "core/sort.h"
#include "2d/sprite.h"

BEGIN_SCRIPT_CLASS_INHERITED(EntityComponentParticlesSystem,EntityComponent)
	SCRIPT_VOID_METHOD3(LoadSprite,Str,Str,Bool)
	SCRIPT_VOID_METHOD2(SetEmitterDelayBeforeSpawn,Float,Float)
	SCRIPT_VOID_METHOD2(SetEmitterLifeTime,Float,Float)
	SCRIPT_VOID_METHOD2(SetParticleCountPerSpawn,S32,S32)
	SCRIPT_VOID_METHOD2(SetParticleSpawnDelay,Float,Float)
	SCRIPT_VOID_METHOD2(SetParticleLifetime,Float,Float)
	SCRIPT_VOID_METHOD2(SetParticleSpeed,Float,Float)
	SCRIPT_VOID_METHOD2(SetParticleAcceleration,Float,Float)
	SCRIPT_VOID_METHOD2(SetParticleRotationSpeed,Float,Float)
	SCRIPT_VOID_METHOD2(SetParticleRotationAcceleration,Float,Float)
	SCRIPT_VOID_METHOD1(SetParticleDirection,Vec3f)
	SCRIPT_VOID_METHOD2(SetParticleSpread,Vec3f,Vec3f)
	SCRIPT_VOID_METHOD2(SetParticleAngleDirection,Float,Float)
	SCRIPT_VOID_METHOD2(SetParticleInitialRotation,Float,Float)
	SCRIPT_VOID_METHOD3(AddParticleSizeKey,Float,Float,Float)					
	SCRIPT_VOID_METHOD3(AddParticleColorKey,Color,Color,Float)	
	SCRIPT_VOID_METHOD3(AddParticleAlphaKey,Float,Float,Float)		
	SCRIPT_VOID_METHOD1(SetRemoveAtEnd,Bool)
	SCRIPT_VOID_METHOD1(SimulateUpdate,Float)
	SCRIPT_VOID_METHOD(Pause)
	SCRIPT_VOID_METHOD(Resume)
END_SCRIPT_CLASS

DEFINE_COMPONENT_CLASS(ParticlesSystem)
END_DEFINE_COMPONENT

EntityComponentParticlesSystem::EntityComponentParticlesSystem()
{
	m_Material = RendererDatas::GetInstance().CreateMaterial();
	m_EmitterDelayBeforeSpawn.Set(0.f,0.f);
	m_EmitterLifeTime.Set(-1.f,-1.f);
	m_ParticleCountPerSpawn.Set(1,1);
	m_ParticleSpawnDelay.Set(.5f,.5f);
	m_ParticleLifeTime.Set(1.f,1.f);
	m_ParticleSpeed.Set(2.f,2.f);
	m_ParticleAcceleration.Set(0.f,0.f);
	m_ParticleRotationSpeed.Set(0.f,0.f);
	m_ParticleRotationAcceleration.Set(0.f,0.f);
	m_ParticleDirection = -Vec3f::YAxis;
	m_ParticleSpreadX.Set(0.f,0.f);
	m_ParticleSpreadY.Set(0.f,0.f);
	m_ParticleSpreadZ.Set(0.f,0.f);
	m_ParticleAngleDirection.Set(0.f,0.f);
	m_ParticleInitialRotation.Set(0.f,0.f);
	m_fSpriteMaxTime = 0.f;
	m_iSpriteAnimId = -1;
	m_bSpriteLoop = FALSE;
	m_bPaused = FALSE;
	m_bRemoveAtEnd = TRUE;
	m_fSimulateUpdate = 0.f;
}

//------------------------------

EntityComponentParticlesSystem::~EntityComponentParticlesSystem()
{
}

//------------------------------

void	EntityComponentParticlesSystem::Initialize()
{
	SUPER::Initialize();
	if( m_ParticleSizes.GetCount() == 0 )
		AddParticleSizeKey(100.f,100.f,0.f);
	if( m_ParticleColors.GetCount() == 0 )
		AddParticleColorKey(Color::White,Color::White,0.f);
	if( m_ParticleAlphas.GetCount() == 0 )
		AddParticleAlphaKey(1.f,1.f,0.f);

	SortAsc(m_ParticleSizes);
	SortAsc(m_ParticleColors);
	SortAsc(m_ParticleAlphas);

	m_SpawnDelay = m_EmitterDelayBeforeSpawn.GetRandomValue();
	m_fMaxLifeTime = m_EmitterLifeTime.GetRandomValue();
	m_fLifeTime = m_fMaxLifeTime;

	m_Mesh = Primitive3D::Create(Str("particlesystem_%s", GetEntity()->GetName().GetStr().GetArray()), TRUE);

	while(m_fSimulateUpdate > 0.f )
	{
		Float dTime = Min(m_fSimulateUpdate,0.02f);
		Update(dTime);
		m_fSimulateUpdate -= dTime;
	}
}

//------------------------------

void EntityComponentParticlesSystem::Update(Float _dTime)
{
	PROFILER_Begin("COMP_Particle");

	// paused
	if( m_bPaused )
		return;

	// update particles
	for(S32 i=m_Particles.GetCount()-1;i>=0;i--)
	{
		UpdateParticle(m_Particles[i],_dTime);
		if( m_Particles[i].m_fLife <= 0.f )
			m_Particles.RemoveItems(i,1);
	}

	// geometry
	CreateGeometry();
	
	// update lifetime
	if( m_fMaxLifeTime > 0.f )
	{
		m_fLifeTime -= _dTime;
		if( m_fLifeTime < 0.f )
		{
			if( m_Particles.GetCount() == 0 && m_bRemoveAtEnd )
				World::GetInstance().RemoveEntity(GetEntity());
			return;
		}
	}

	// update spawn
	m_SpawnDelay -= _dTime;
	if( m_SpawnDelay < 0.f )
	{
		S32 count = m_ParticleCountPerSpawn.GetRandomValue();
		for(S32 i=0; i<count; i++)
			SpawnParticle();

		m_SpawnDelay = m_ParticleSpawnDelay.GetRandomValue();
	}
	PROFILER_End();
}

//------------------------------

void EntityComponentParticlesSystem::Draw(Renderer*	_renderer)
{
	if( GetSprite() )
		_renderer->SetActiveMaterial(GetSprite()->GetMaterial(),GetEntity()->GetColor());
	else
		_renderer->SetActiveMaterial(m_Material,GetEntity()->GetColor());

	_renderer->DrawGeometry(Geometry_TrianglesList,GetEntity()->GetWorldPos(),GetEntity()->GetWorldRot(),m_Mesh,GetEntity()->GetWorldScale());
	_renderer->SetActiveMaterial(NULL);
}

//------------------------------

void EntityComponentParticlesSystem::LoadSprite(const Str& _path,const Str& _anim, Bool _loop)
{
Str luaPath = _path;
	luaPath += ".lua";
	m_Sprite = ResourceManager::GetInstance().GetResourceFromFile<Sprite>(luaPath);
	if( m_Sprite.IsValid() )
	{
		m_nSpriteAnim = _anim;
		m_fSpriteMaxTime = GetSprite()->GetAnimDuration(m_nSpriteAnim);
		m_iSpriteAnimId = GetSprite()->GetAnimId(m_nSpriteAnim);
		m_bSpriteLoop = _loop;
	}
}

//------------------------------

void EntityComponentParticlesSystem::SpawnParticle()
{
	Particle& particle = m_Particles.AddLastItem(Particle());

	// square
	particle.m_vPos.x = m_ParticleSpreadX.GetRandomValue();
	particle.m_vPos.y = m_ParticleSpreadY.GetRandomValue();
	particle.m_vPos.z = m_ParticleSpreadZ.GetRandomValue();

	Quat rotDir;
	rotDir.SetFromAxis(m_ParticleAngleDirection.GetRandomValue(),Vec3f::ZAxis);
	Vec3f direction = rotDir * m_ParticleDirection;
	direction.Normalize();
	particle.m_vSpeed = direction * m_ParticleSpeed.GetRandomValue();
	particle.m_fAccel = m_ParticleAcceleration.GetRandomValue();

	Float angle = acosf(-direction.y);
	if( direction.x < 0.f )
		angle = -angle;
	
	particle.m_qRot.SetFromAxis(m_ParticleInitialRotation.GetRandomValue() + angle,Vec3f::ZAxis);
	particle.m_qRotSpeed.SetFromAxis(m_ParticleRotationSpeed.GetRandomValue(),Vec3f::ZAxis);
	particle.m_SizeRatio = ((Float)rand()) / ((Float)RAND_MAX);
	particle.m_ColorRatio = ((Float)rand()) / ((Float)RAND_MAX);
	particle.m_AlphaRatio = ((Float)rand()) / ((Float)RAND_MAX);
	particle.m_fSize = GetParticleSize(0.f,particle.m_SizeRatio);
	particle.m_Color = GetParticleColor(0.f,particle.m_ColorRatio);
	particle.m_Color.a = GetParticleAlpha(0.f,particle.m_AlphaRatio);

	particle.m_fMaxLife = m_ParticleLifeTime.GetRandomValue(); 
	particle.m_fLife = particle.m_fMaxLife;
	particle.m_fSpriteProgress = 0.f;
}

//------------------------------

Float EntityComponentParticlesSystem::GetParticleSize(Float _progress, Float _ratio)
{
	DEBUG_Require(_progress>=0.f && _progress<= 1.f);

	if( _progress <= m_ParticleSizes[0].m_Ratio )
		return m_ParticleSizes[0].m_Range.GetValue(_ratio);
	else if( _progress >= m_ParticleSizes[m_ParticleSizes.GetCount()-1].m_Ratio )
		return m_ParticleSizes[m_ParticleSizes.GetCount()-1].m_Range.GetValue(_ratio);
	else
	{
		for(U32 i=0;i<m_ParticleSizes.GetCount()-1;i++)
		{
			if( _progress <= m_ParticleSizes[i+1].m_Ratio )
			{
				Float v1 = m_ParticleSizes[i].m_Range.GetValue(_ratio);
				Float r1 = m_ParticleSizes[i].m_Ratio;
				Float v2 = m_ParticleSizes[i+1].m_Range.GetValue(_ratio);
				Float r2 = m_ParticleSizes[i+1].m_Ratio;
				return LinearInterpolation(v1,v2,(_progress - r1 ) / ( r2 - r1 ) );
			}
		}
	}

	return m_ParticleSizes[0].m_Range.GetValue(_ratio);
}

//------------------------------

Color EntityComponentParticlesSystem::GetParticleColor(Float _progress, Float _ratio)
{
	DEBUG_Require(_progress>=0.f && _progress<= 1.f);

	if( _progress <= m_ParticleColors[0].m_Ratio )
		return m_ParticleColors[0].m_Range.GetValue(_ratio);
	else if( _progress >= m_ParticleColors[m_ParticleColors.GetCount()-1].m_Ratio )
		return m_ParticleColors[m_ParticleColors.GetCount()-1].m_Range.GetValue(_ratio);
	else
	{
		for(U32 i=0;i<m_ParticleColors.GetCount()-1;i++)
		{
			if( _progress <= m_ParticleColors[i+1].m_Ratio )
			{
				Color v1 = m_ParticleColors[i].m_Range.GetValue(_ratio);
				Float r1 = m_ParticleColors[i].m_Ratio;
				Color v2 = m_ParticleColors[i+1].m_Range.GetValue(_ratio);
				Float r2 = m_ParticleColors[i+1].m_Ratio;
				return LinearInterpolation(v1,v2,(_progress - r1 ) / ( r2 - r1 ) );
			}
		}
	}

	return m_ParticleColors[0].m_Range.GetValue(_ratio);
}

//------------------------------

Float EntityComponentParticlesSystem::GetParticleAlpha(Float _progress, Float _ratio)
{
	DEBUG_Require(_progress>=0.f && _progress<= 1.f);

	if( _progress <= m_ParticleAlphas[0].m_Ratio )
		return m_ParticleAlphas[0].m_Range.GetValue(_ratio);
	else if( _progress >= m_ParticleAlphas[m_ParticleAlphas.GetCount()-1].m_Ratio )
		return m_ParticleAlphas[m_ParticleAlphas.GetCount()-1].m_Range.GetValue(_ratio);
	else
	{
		for(U32 i=0;i<m_ParticleAlphas.GetCount()-1;i++)
		{
			if( _progress <= m_ParticleAlphas[i+1].m_Ratio )
			{
				Float v1 = m_ParticleAlphas[i].m_Range.GetValue(_ratio);
				Float r1 = m_ParticleAlphas[i].m_Ratio;
				Float v2 = m_ParticleAlphas[i+1].m_Range.GetValue(_ratio);
				Float r2 = m_ParticleAlphas[i+1].m_Ratio;
				return LinearInterpolation(v1,v2,(_progress - r1 ) / ( r2 - r1 ) );
			}
		}
	}

	return m_ParticleAlphas[0].m_Range.GetValue(_ratio);
}

//------------------------------

void EntityComponentParticlesSystem::UpdateParticle(Particle& _particle, Float _dTime)
{
	_particle.m_fLife = Max(_particle.m_fLife-_dTime,0.f);
	
	Float ratio = 1.f - (_particle.m_fLife / _particle.m_fMaxLife);
	_particle.m_fSize = GetParticleSize(ratio,_particle.m_SizeRatio);
	_particle.m_Color = GetParticleColor(ratio,_particle.m_ColorRatio);
	_particle.m_Color.a = GetParticleAlpha(ratio,_particle.m_AlphaRatio);

	Float oldSpeed = _particle.m_vSpeed.GetLength();
	Float newSpeed = oldSpeed + _particle.m_fAccel * _dTime;
	_particle.m_vSpeed *= newSpeed / oldSpeed;
	_particle.m_vPos += _particle.m_vSpeed * _dTime;
	_particle.m_qRot *= _particle.m_qRotSpeed * _dTime;

	_particle.m_fSpriteProgress += _dTime;
	if( _particle.m_fSpriteProgress >= m_fSpriteMaxTime )
		if( m_bSpriteLoop )
			_particle.m_fSpriteProgress -= m_fSpriteMaxTime;
		else
			_particle.m_fSpriteProgress = m_fSpriteMaxTime;
}

//------------------------------

void EntityComponentParticlesSystem::CreateGeometry()
{
	U16 count = (U16)m_Particles.GetCount();
	VertexBufferRef vb;
	IndexBufferRef ib;
	m_Mesh->Lock(vb,ib);
	vb->SetVertexCount(count*4);
	ib->m_Array.SetItemCount(count*6);

	for(U16 i=0; i<m_Particles.GetCount(); i++)
	{
		const Particle& particle = m_Particles[i];

		Vec3f pos = Vec3f::Zero;
		Vec3f size = Vec3f::One;
		Vec2f uvmin,uvmax;
		uvmin.x = 0.f;
		uvmin.y = 0.f;
		uvmax.x = 1.f;
		uvmax.y = 1.f;
		if( GetSprite() )
		{
			S32 frameidx = (S32)(particle.m_fSpriteProgress * 30.f - Float_Eps);
			GetSprite()->GetFrameUVs(m_iSpriteAnimId,frameidx,pos,size,uvmin,uvmax);
		}
		else if( m_Material->GetTexture() )
		{
			uvmin = m_Material->GetUvMin();
			uvmax = m_Material->GetUvMax();
		}

		Vec3f leftaxis = particle.m_qRot * Vec3f::XAxis;
		Vec3f upaxis = particle.m_qRot * Vec3f::YAxis;

		Float halfSize = particle.m_fSize*.5f;
		Vec3f left = leftaxis * halfSize;
		Vec3f up = upaxis * halfSize;

		Vec3f refpos = particle.m_vPos + particle.m_qRot * pos;
		Vec3f v0 = particle.m_vPos - left * size.x - up * size.y;
		Vec3f v1 = particle.m_vPos - left * size.x + up * size.y;
		Vec3f v2 = particle.m_vPos + left * size.x - up * size.y;
		Vec3f v3 = particle.m_vPos + left * size.x + up * size.y;
		
		U16 startvtx = i*4;
		vb->SetVertexAtIndex(startvtx,v0,Vec2f(uvmin.x,uvmax.y),Vec3f::ZAxis,particle.m_Color);
		vb->SetVertexAtIndex(startvtx+1,v1,Vec2f(uvmin.x,uvmin.y),Vec3f::ZAxis,particle.m_Color);
		vb->SetVertexAtIndex(startvtx+2,v2,Vec2f(uvmax.x,uvmax.y),Vec3f::ZAxis,particle.m_Color);
		vb->SetVertexAtIndex(startvtx+3,v3,Vec2f(uvmax.x,uvmin.y),Vec3f::ZAxis,particle.m_Color);

		U32 startidx = i*6;
		ib->m_Array[startidx] = startvtx;
		ib->m_Array[startidx+1] = startvtx+1;
		ib->m_Array[startidx+2] = startvtx+2;
		ib->m_Array[startidx+3] = startvtx+1;
		ib->m_Array[startidx+4] = startvtx+2;
		ib->m_Array[startidx+5] = startvtx+3;
	}

	m_Mesh->Unlock();
}