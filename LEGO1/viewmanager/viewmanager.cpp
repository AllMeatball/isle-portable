#include "viewmanager.h"

#include "mxdirectx/mxstopwatch.h"
#include "tgl/d3drm/impl.h"
#include "viewlod.h"

#include <vec.h>

DECOMP_SIZE_ASSERT(ViewManager, 0x1bc)

// GLOBAL: LEGO1 0x100dbc78
int g_boundingBoxCornerMap[8][3] =
	{{0, 0, 0}, {0, 0, 1}, {0, 1, 0}, {1, 0, 0}, {0, 1, 1}, {1, 0, 1}, {1, 1, 0}, {1, 1, 1}};

// GLOBAL: LEGO1 0x100dbcd8
int g_planePointIndexMap[18] = {0, 1, 5, 6, 2, 3, 3, 0, 4, 1, 2, 6, 0, 3, 2, 4, 5, 6};

// GLOBAL: LEGO1 0x10101050
float g_LODScaleFactor = 4.0F;

// GLOBAL: LEGO1 0x10101054
float g_minLODThreshold = 0.00097656297;

// GLOBAL: LEGO1 0x10101058
int g_maxLODLevels = 6;

// GLOBAL: LEGO1 0x1010105c
float g_unk0x1010105c = 0.000125F;

// GLOBAL: LEGO1 0x10101060
float g_elapsedSeconds = 0;

inline void SetAppData(ViewROI* p_roi, LPD3DRM_APPDATA data);
inline undefined4 GetD3DRM(IDirect3DRM2*& d3drm, Tgl::Renderer* pRenderer);
inline undefined4 GetFrame(IDirect3DRMFrame2*& frame, Tgl::Group* scene);

// FUNCTION: LEGO1 0x100a5eb0
ViewManager::ViewManager(Tgl::Renderer* pRenderer, Tgl::Group* scene, const OrientableROI* point_of_view)
	: scene(scene), flags(c_bit1 | c_bit2 | c_bit3 | c_bit4)
{
	SetPOVSource(point_of_view);
	prev_render_time = 0.09;
	GetD3DRM(d3drm, pRenderer);
	GetFrame(frame, scene);
	width = 0.0;
	height = 0.0;
	view_angle = 0.0;
	pov.SetIdentity();
	front = 0.0;
	back = 0.0;

	memset(transformed_points, 0, sizeof(transformed_points));
	seconds_allowed = 1.0;
}

// FUNCTION: LEGO1 0x100a60c0
ViewManager::~ViewManager()
{
	SetPOVSource(NULL);
}

// FUNCTION: LEGO1 0x100a6150
// FUNCTION: BETA10 0x10172164
unsigned int ViewManager::IsBoundingBoxInFrustum(const BoundingBox& p_bounding_box)
{
	const Vector3* box[] = {&p_bounding_box.Min(), &p_bounding_box.Max()};

	float und[8][3];
	int i, j, k;

	for (i = 0; i < 8; i++) {
		for (j = 0; j < 3; j++) {
			und[i][j] = box[g_boundingBoxCornerMap[i][j]]->operator[](j);
		}
	}

	for (i = 0; i < 6; i++) {
		for (k = 0; k < 8; k++) {
			if (frustum_planes[i][0] * und[k][0] + frustum_planes[i][2] * und[k][2] + frustum_planes[i][1] * und[k][1] +
					frustum_planes[i][3] >=
				0.0f) {
				break;
			}
		}

		if (k == 8) {
			return FALSE;
		}
	}

	return TRUE;
}

// FUNCTION: LEGO1 0x100a6410
void ViewManager::Remove(ViewROI* p_roi)
{
	for (CompoundObject::iterator it = rois.begin(); it != rois.end(); it++) {
		if (*it == p_roi) {
			rois.erase(it);

			if (p_roi->GetUnknown0xe0() >= 0) {
				RemoveROIDetailFromScene(p_roi);
			}

			const CompoundObject* comp = p_roi->GetComp();

			if (comp != NULL) {
				for (CompoundObject::const_iterator it = comp->begin(); !(it == comp->end()); it++) {
					if (((ViewROI*) *it)->GetUnknown0xe0() >= 0) {
						RemoveROIDetailFromScene((ViewROI*) *it);
					}
				}
			}

			return;
		}
	}
}

// FUNCTION: LEGO1 0x100a64d0
void ViewManager::RemoveAll(ViewROI* p_roi)
{
	if (p_roi == NULL) {
		for (CompoundObject::iterator it = rois.begin(); it != rois.end(); it++) {
			RemoveAll((ViewROI*) *it);
		}

		rois.erase(rois.begin(), rois.end());
	}
	else {
		if (p_roi->GetUnknown0xe0() >= 0) {
			RemoveROIDetailFromScene(p_roi);
		}

		p_roi->SetUnknown0xe0(-1);
		const CompoundObject* comp = p_roi->GetComp();

		if (comp != NULL) {
			for (CompoundObject::const_iterator it = comp->begin(); !(it == comp->end()); it++) {
				if ((ViewROI*) *it != NULL) {
					RemoveAll((ViewROI*) *it);
				}
			}
		}
	}
}

// FUNCTION: LEGO1 0x100a65b0
void ViewManager::UpdateROIDetailBasedOnLOD(ViewROI* p_roi, int p_und)
{
	if (p_roi->GetLODCount() <= p_und) {
		p_und = p_roi->GetLODCount() - 1;
	}

	int unk0xe0 = p_roi->GetUnknown0xe0();

	if (unk0xe0 == p_und) {
		return;
	}

	Tgl::Group* group = p_roi->GetGeometry();
	Tgl::MeshBuilder* meshBuilder;
	ViewLOD* lod;

	if (unk0xe0 < 0) {
		lod = (ViewLOD*) p_roi->GetLOD(p_und);

		if (lod->GetUnknown0x08() & ViewLOD::c_bit4) {
			scene->Add(group);
			SetAppData(p_roi, reinterpret_cast<LPD3DRM_APPDATA>(p_roi));
		}
	}
	else {
		lod = (ViewLOD*) p_roi->GetLOD(unk0xe0);

		if (lod != NULL) {
			meshBuilder = lod->GetMeshBuilder();

			if (meshBuilder != NULL) {
				group->Remove(meshBuilder);
			}
		}

		lod = (ViewLOD*) p_roi->GetLOD(p_und);
	}

	if (lod->GetUnknown0x08() & ViewLOD::c_bit4) {
		meshBuilder = lod->GetMeshBuilder();

		if (meshBuilder != NULL) {
			group->Add(meshBuilder);
			SetAppData(p_roi, reinterpret_cast<LPD3DRM_APPDATA>(p_roi));
			p_roi->SetUnknown0xe0(p_und);
			return;
		}
	}

	p_roi->SetUnknown0xe0(-1);
}

// FUNCTION: LEGO1 0x100a66a0
void ViewManager::RemoveROIDetailFromScene(ViewROI* p_roi)
{
	const ViewLOD* lod = (const ViewLOD*) p_roi->GetLOD(p_roi->GetUnknown0xe0());

	if (lod != NULL) {
		const Tgl::MeshBuilder* meshBuilder = NULL;
		Tgl::Group* roiGeometry = p_roi->GetGeometry();

		meshBuilder = lod->GetMeshBuilder();

		if (meshBuilder != NULL) {
			roiGeometry->Remove(meshBuilder);
		}

		scene->Remove(roiGeometry);
	}

	p_roi->SetUnknown0xe0(-1);
}

// FUNCTION: LEGO1 0x100a66f0
// FUNCTION: BETA10 0x1017297f
inline void ViewManager::ManageVisibilityAndDetailRecursively(ViewROI* p_from, int p_und)
{
	assert(p_from);

	if (!p_from->GetVisibility() && p_und != -2) {
		ManageVisibilityAndDetailRecursively(p_from, -2);
	}
	else {
		const CompoundObject* comp = p_from->GetComp();

		if (p_und == -1) {
			if (p_from->GetWorldBoundingSphere().Radius() > 0.001F) {
				float und = ProjectedSize(p_from->GetWorldBoundingSphere());

				if (und < seconds_allowed * g_unk0x1010105c) {
					if (p_from->GetUnknown0xe0() != -2) {
						ManageVisibilityAndDetailRecursively(p_from, -2);
					}

					return;
				}
				else {
					p_und = CalculateLODLevel(und, RealtimeView::GetUserMaxLodPower() * seconds_allowed, p_from);
				}
			}
		}

		if (p_und == -2) {
			if (p_from->GetUnknown0xe0() >= 0) {
				RemoveROIDetailFromScene(p_from);
				p_from->SetUnknown0xe0(-2);
			}

			if (comp != NULL) {
				for (CompoundObject::const_iterator it = comp->begin(); it != comp->end(); it++) {
					ManageVisibilityAndDetailRecursively((ViewROI*) *it, p_und);
				}
			}
		}
		else if (comp == NULL) {
			if (p_from->GetLODs() != NULL && p_from->GetLODCount() > 0) {
				UpdateROIDetailBasedOnLOD(p_from, p_und);
			}
		}
		else {
			p_from->SetUnknown0xe0(-1);

			for (CompoundObject::const_iterator it = comp->begin(); it != comp->end(); it++) {
				ManageVisibilityAndDetailRecursively((ViewROI*) *it, p_und);
			}
		}
	}
}

// FUNCTION: LEGO1 0x100a6930
void ViewManager::Update(float p_previousRenderTime, float)
{
	MxStopWatch stopWatch;
	stopWatch.Start();

	prev_render_time = p_previousRenderTime;
	flags |= c_bit1;

	if (flags & c_bit3) {
		CalculateFrustumTransformations();
	}
	else if (flags & c_bit2) {
		UpdateViewTransformations();
	}

	for (CompoundObject::iterator it = rois.begin(); it != rois.end(); it++) {
		ManageVisibilityAndDetailRecursively((ViewROI*) *it, -1);
	}

	stopWatch.Stop();
	g_elapsedSeconds = stopWatch.ElapsedSeconds();
}

inline int ViewManager::CalculateFrustumTransformations()
{
	flags &= ~c_bit3;

	if (height == 0.0F || front == 0.0F) {
		return -1;
	}
	else {
		float fVar7 = tan(view_angle / 2.0F);
		view_area_at_one = fVar7 * fVar7 * 4.0F;

		float fVar1 = front * fVar7;
		float fVar2 = (width / height) * fVar1;
		float uVar6 = front;
		float fVar3 = back + front;
		float fVar4 = fVar3 / front;
		float fVar5 = fVar4 * fVar1;
		fVar4 = fVar4 * fVar2;

		float* frustumVertices = (float*) this->frustum_vertices;

		// clang-format off
		*frustumVertices = fVar2; frustumVertices++;
		*frustumVertices = fVar1; frustumVertices++;
		*frustumVertices = uVar6; frustumVertices++;
		*frustumVertices = fVar2; frustumVertices++;
		*frustumVertices = -fVar1; frustumVertices++;
		*frustumVertices = uVar6; frustumVertices++;
		*frustumVertices = -fVar2; frustumVertices++;
		*frustumVertices = -fVar1; frustumVertices++;
		*frustumVertices = uVar6; frustumVertices++;
		*frustumVertices = -fVar2; frustumVertices++;
		*frustumVertices = fVar1; frustumVertices++;
		*frustumVertices = uVar6; frustumVertices++;
		*frustumVertices = fVar4; frustumVertices++;
		*frustumVertices = fVar5; frustumVertices++;
		*frustumVertices = fVar3; frustumVertices++;
		*frustumVertices = fVar4; frustumVertices++;
		*frustumVertices = -fVar5; frustumVertices++;
		*frustumVertices = fVar3; frustumVertices++;
		*frustumVertices = -fVar4; frustumVertices++;
		*frustumVertices = -fVar5; frustumVertices++;
		*frustumVertices = fVar3; frustumVertices++;
		*frustumVertices = -fVar4; frustumVertices++;
		*frustumVertices = fVar5; frustumVertices++;
		*frustumVertices = fVar3;
		// clang-format on

		UpdateViewTransformations();
		return 0;
	}
}

// FUNCTION: BETA10 0x10172be5
inline int ViewManager::CalculateLODLevel(float p_und1, float p_und2, ViewROI* from)
{
	int result;

	assert(from);

	if (IsROIVisibleAtLOD(from) != 0) {
		if (p_und1 < g_minLODThreshold) {
			return 0;
		}
		else {
			result = 1;
		}
	}
	else {
		result = 0;
	}

	for (float i = p_und2; result < g_maxLODLevels; result++) {
		if (i >= p_und1) {
			break;
		}

		i *= g_LODScaleFactor;
	}

	return result;
}

// FUNCTION: BETA10 0x10172cb0
inline int ViewManager::IsROIVisibleAtLOD(ViewROI* p_roi)
{
	const LODListBase* lods = p_roi->GetLODs();

	if (lods != NULL && lods->Size() > 0) {
		if (((ViewLOD*) p_roi->GetLOD(0))->GetUnknown0x08Test8()) {
			return 1;
		}
		else {
			return 0;
		}
	}

	const CompoundObject* comp = p_roi->GetComp();

	if (comp != NULL) {
		for (CompoundObject::const_iterator it = comp->begin(); it != comp->end(); it++) {
			const LODListBase* lods = ((ViewROI*) *it)->GetLODs();

			if (lods != NULL && lods->Size() > 0) {
				if (((ViewLOD*) ((ViewROI*) *it)->GetLOD(0))->GetUnknown0x08Test8()) {
					return 1;
				}
				else {
					return 0;
				}
			}
		}
	}

	return 0;
}

// FUNCTION: LEGO1 0x100a6b90
void ViewManager::UpdateViewTransformations()
{
	flags &= ~c_bit2;

	int i, j, k;

	for (i = 0; i < 8; i++) {
		for (j = 0; j < 3; j++) {
			transformed_points[i][j] = pov[3][j];

			for (k = 0; k < 3; k++) {
				transformed_points[i][j] += pov[k][j] * frustum_vertices[i][k];
			}
		}
	}

	for (i = 0; i < 6; i++) {
		Vector3 a(transformed_points[g_planePointIndexMap[i * 3]]);
		Vector3 b(transformed_points[g_planePointIndexMap[i * 3 + 1]]);
		Vector3 c(transformed_points[g_planePointIndexMap[i * 3 + 2]]);
		Mx3DPointFloat x;
		Mx3DPointFloat y;
		Vector3 normal(frustum_planes[i]);

		x = c;
		x -= b;

		y = a;
		y -= b;

		normal.EqualsCross(x, y);
		normal.Unitize();

		frustum_planes[i][3] = -normal.Dot(normal, a);
	}

	flags |= c_bit4;
}

// FUNCTION: LEGO1 0x100a6d50
void ViewManager::SetResolution(int width, int height)
{
	flags |= c_bit3;
	this->width = width;
	this->height = height;
}

// FUNCTION: LEGO1 0x100a6d70
void ViewManager::SetFrustrum(float fov, float front, float back)
{
	this->front = front;
	this->back = back;
	flags |= c_bit3;
	view_angle = fov * 0.017453292519944444;
}

// FUNCTION: LEGO1 0x100a6da0
void ViewManager::SetPOVSource(const OrientableROI* point_of_view)
{
	if (point_of_view != NULL) {
		pov = point_of_view->GetLocal2World();
		flags |= c_bit2;
	}
}

// FUNCTION: LEGO1 0x100a6dc0
// FUNCTION: BETA10 0x101739b8
float ViewManager::ProjectedSize(const BoundingSphere& p_bounding_sphere)
{
	// The algorithm projects the radius of bounding sphere onto the perpendicular
	// plane one unit in front of the camera. That value is simply the ratio of the
	// radius to the distance from the camera to the sphere center. The projected size
	// is then the ratio of the area of that projected circle to the view surface area
	// at Z == 1.0.
	//
	float sphere_projected_area = 3.14159265359 * p_bounding_sphere.Radius() * p_bounding_sphere.Radius();
	float square_dist_to_sphere = DISTSQRD3(p_bounding_sphere.Center(), pov[3]);
	return sphere_projected_area / view_area_at_one / square_dist_to_sphere;
}

// FUNCTION: LEGO1 0x100a6e00
ViewROI* ViewManager::Pick(Tgl::View* p_view, unsigned int x, unsigned int y)
{
	LPDIRECT3DRMPICKEDARRAY picked = NULL;
	ViewROI* result = NULL;
	TglImpl::ViewImpl* view = (TglImpl::ViewImpl*) p_view;
	IDirect3DRMViewport* d3drm = view->ImplementationData();

	if (d3drm->Pick(x, y, &picked) != D3DRM_OK) {
		return NULL;
	}

	if (picked != NULL) {
		if (picked->GetSize() != 0) {
			LPDIRECT3DRMVISUAL visual;
			LPDIRECT3DRMFRAMEARRAY frameArray;
			D3DRMPICKDESC desc;

			if (picked->GetPick(0, &visual, &frameArray, &desc) == D3DRM_OK) {
				if (frameArray != NULL) {
					int size = frameArray->GetSize();

					if (size > 1) {
						for (int i = 1; i < size; i++) {
							LPDIRECT3DRMFRAME frame = NULL;

							if (frameArray->GetElement(i, &frame) == D3DRM_OK) {
								result = (ViewROI*) frame->GetAppData();

								if (result != NULL) {
									frame->Release();
									break;
								}

								frame->Release();
							}
						}
					}

					visual->Release();
					frameArray->Release();
				}
			}
		}

		picked->Release();
	}

	return result;
}

inline void SetAppData(ViewROI* p_roi, LPD3DRM_APPDATA data)
{
	IDirect3DRMFrame2* frame = NULL;

	if (GetFrame(frame, p_roi->GetGeometry()) == 0) {
		frame->SetAppData(data);
	}
}

inline undefined4 GetD3DRM(IDirect3DRM2*& d3drm, Tgl::Renderer* pRenderer)
{
	d3drm = ((TglImpl::RendererImpl*) pRenderer)->ImplementationData();
	return 0;
}

inline undefined4 GetFrame(IDirect3DRMFrame2*& frame, Tgl::Group* scene)
{
	frame = ((TglImpl::GroupImpl*) scene)->ImplementationData();
	return 0;
}
