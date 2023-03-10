#ifndef __COMPONENT_PARTICLES_SYSTEM_H__
#define __COMPONENT_PARTICLES_SYSTEM_H__

#include "component.h"
#include "3d/vertex_buffer.h"
#include "3d/index_buffer.h"
class Sprite;

BEGIN_COMPONENT_CLASS(ParticlesSystem)
	DECLARE_SCRIPT_CLASS(EntityComponentParticlesSystem)
public:	
	template<typename T>
	struct Range
	{
		T m_Min;
		T m_Max;

		void Set(const T& _min, const T& _max)	
		{
			m_Min = _min;
			m_Max = _max;
		}
	
		T	GetValue(Float _ratio) const 	{ return LinearInterpolation(m_Min,m_Max,_ratio); }
		T	GetRandomValue() const 			{ return GetValue(((Float)rand()) / ((Float)RAND_MAX)); }
	};

	template<typename T>
	struct Key
	{
		Range<T>	m_Range;
		Float 		m_Ratio;

		Key()
		{
		}

		Key(const T& _min, const T& _max, Float _ratio)
		{
			m_Range.Set(_min,_max);
			m_Ratio = _ratio;
		}
	};

	EntityComponentParticlesSystem();
	~EntityComponentParticlesSystem();

	virtual Bool IsUpdater() const	{ return TRUE; }
	virtual Bool IsDrawer()	const	{ return TRUE; }
	virtual Bool ReceiveEvent()	const { return FALSE; }

	virtual void	Initialize();
	virtual void	Update(Float _dTime);
	virtual void	Draw(Renderer*	_renderer);

	void LoadSprite(const Str& _path, const Str& _anim, Bool _loop);

	Bool IsPlaying() const { return m_fLifeTime > 0.f || m_Particles.GetCount() != 0; }

	void Pause()	{ m_bPaused = TRUE; }
	void Resume()	{ m_bPaused = FALSE; }
	
	void SetEmitterDelayBeforeSpawn(Float _min, Float _max)					{ m_EmitterDelayBeforeSpawn.Set(_min,_max); }
	void SetEmitterLifeTime(Float _min, Float _max)							{ m_EmitterLifeTime.Set(_min,_max); }
	void SetParticleCountPerSpawn(S32 _min, S32 _max)						{ m_ParticleCountPerSpawn.Set(_min,_max); }
	void SetParticleSpawnDelay(Float _min, Float _max)						{ m_ParticleSpawnDelay.Set(_min,_max); }
	void SetParticleLifetime(Float _min, Float _max)						{ m_ParticleLifeTime.Set(_min,_max); }
	void SetParticleSpeed(Float _min, Float _max)							{ m_ParticleSpeed.Set(_min,_max); }
	void SetParticleAcceleration(Float _min, Float _max)					{ m_ParticleAcceleration.Set(_min,_max); }
	void SetParticleRotationSpeed(Float _min, Float _max)					{ m_ParticleRotationSpeed.Set(MATH_DEG_TO_RAD(_min), MATH_DEG_TO_RAD(_max)); }
	void SetParticleRotationAcceleration(Float _min, Float _max)			{ m_ParticleRotationAcceleration.Set(MATH_DEG_TO_RAD(_min), MATH_DEG_TO_RAD(_max)); }
	void SetParticleDirection(const Vec3f& _dir)							{ m_ParticleDirection = _dir; }
	void SetParticleSpread(const Vec3f& _min, const Vec3f& _max)			{ m_ParticleSpreadX.Set(_min.x,_max.x); m_ParticleSpreadY.Set(_min.y,_max.y); m_ParticleSpreadZ.Set(_min.z,_max.z);}
	void SetParticleAngleDirection(Float _min, Float _max)					{ m_ParticleAngleDirection.Set(MATH_DEG_TO_RAD(_min), MATH_DEG_TO_RAD(_max)); }
	void SetParticleInitialRotation(Float _min, Float _max)					{ m_ParticleInitialRotation.Set(MATH_DEG_TO_RAD(_min), MATH_DEG_TO_RAD(_max)); }
	void AddParticleSizeKey(Float _min, Float _max,Float _keyRatio)					{ m_ParticleSizes.AddLastItem(Key<Float>(_min,_max,_keyRatio)); }
	void AddParticleColorKey(const Color& _min, const Color& _max,Float _keyRatio)	{ m_ParticleColors.AddLastItem(Key<Color>(_min,_max,_keyRatio)); }
	void AddParticleAlphaKey(Float _min, Float _max,Float _keyRatio)				{ m_ParticleAlphas.AddLastItem(Key<Float>(_min,_max,_keyRatio)); }
	void SetRemoveAtEnd(Bool _remove)										{ m_bRemoveAtEnd = _remove; }
	void SimulateUpdate(Float _value)										{ m_fSimulateUpdate = _value; }
protected:
	struct Particle
	{
		Vec3f		m_vPos;
		Vec3f 		m_vSpeed;
		Float		m_fAccel;
		Quat 		m_qRot;
		Quat		m_qRotSpeed;
		Float		m_fSize;
		Color		m_Color;
		Float		m_fLife;
		Float		m_fMaxLife;
		Float		m_SizeRatio;
		Float		m_ColorRatio;
		Float		m_AlphaRatio;
		Float		m_fSpriteProgress;

		Particle(){}
	};
	
	void SpawnParticle();
	void UpdateParticle(Particle& _particle, Float _dTime);
	void CreateGeometry();

	Float GetParticleSize(Float _progress, Float _ratio);
	Color GetParticleColor(Float _progress, Float _ratio);
	Float GetParticleAlpha(Float _progress, Float _ratio);

	Sprite*	GetSprite()				{ return (Sprite*)m_Sprite.GetPtr(); }

	Bool					m_bPaused;
	MaterialRef				m_Material;
	Primitive3DRef			m_Mesh;
	ArrayOf<Particle>		m_Particles;
	Float					m_SpawnDelay;
	Float					m_fLifeTime;
	Float					m_fMaxLifeTime;
	Range<Float>			m_EmitterDelayBeforeSpawn;
	Range<Float>			m_EmitterLifeTime;
	Range<S32>				m_ParticleCountPerSpawn;
	Range<Float>			m_ParticleSpawnDelay;
	Range<Float>			m_ParticleLifeTime;
	Range<Float>			m_ParticleSpeed;
	Range<Float>			m_ParticleAcceleration;
	Range<Float>			m_ParticleRotationSpeed;
	Range<Float>			m_ParticleRotationAcceleration;
	Vec3f					m_ParticleDirection;
	Range<Float>			m_ParticleSpreadX;
	Range<Float>			m_ParticleSpreadY;
	Range<Float>			m_ParticleSpreadZ;
	Range<Float>			m_ParticleAngleDirection;
	Range<Float>			m_ParticleInitialRotation;
	ArrayOf< Key<Float> >	m_ParticleSizes;
	ArrayOf< Key<Color> >	m_ParticleColors;
	ArrayOf< Key<Float> >	m_ParticleAlphas;
	ResourceRef				m_Sprite;
	Name					m_nSpriteAnim;
	Float					m_fSpriteMaxTime;
	S32						m_iSpriteAnimId;
	Float					m_fSimulateUpdate;
	Bool					m_bSpriteLoop;
	Bool					m_bRemoveAtEnd;
END_COMPONENT_CLASS                          

template<typename T>
Bool	operator<(const EntityComponentParticlesSystem::Key<T>& _a,const EntityComponentParticlesSystem::Key<T>& _b) { return _a.m_Ratio < _b.m_Ratio; }

#endif
