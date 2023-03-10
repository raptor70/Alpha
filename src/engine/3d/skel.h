#ifndef __SKEL_H__
#define __SKEL_H__

class CBone;
typedef RefTo<CBone>		BoneRef;
typedef ArrayOf<BoneRef>	BoneRefArray;

class CBone
{
	friend class CSkel;
	friend class CSkin;
public:
	CBone(const Name& _name);
	virtual ~CBone();

	Mat44 GetSkelMatrix() const;
	void SetInvShapeMatrix(const Mat44& _mat) { m_InvShapeMatrix = _mat; }
	void SetTransformMatrix(const Mat44& _mat) { m_TransformMatrix = _mat; } 

protected:
	Mat44			m_TransformMatrix;
	Mat44			m_InvShapeMatrix;
	Name			m_Name;
	BoneRefArray	m_aSons;
	CBone*			m_pParent;
};

class CSkel
{
public:
	CSkel();
	virtual ~CSkel();

	CBone*	GetBone(const Name& _name, CBone* _parent = NULL);
	CBone*	AddBone(const Name& _name, const Mat44& _mat, const Name& _parent = Name::Null);

	void	DrawDebug(const Mat44& _worldMat, const CBone* _pBone =NULL) const;
protected:
	BoneRefArray	m_Bones;
};

#endif