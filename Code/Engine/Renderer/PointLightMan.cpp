#include <Engine/StdAfx.h>
#include <Engine/Renderer/PointLightMan.h>
#include <Engine/IPointLight.h>
#include <Engine/Renderer/PointLight.h>
#include <Engine/Renderer/Shaders/Constants.h>
namespace fastbird
{

	PointLightMan::PointLightMan()
	{
	}

	PointLightMan::~PointLightMan()
	{
		for (auto& pl : mPointLights)
		{
			FB_DELETE(pl);
		}
		mPointLights.clear();
	}

	void PointLightMan::Update(float dt)
	{
		static std::vector<unsigned> deleted;
		deleted.reserve(50);

		unsigned i = 0;
		for (auto& pointLight : mPointLights)
		{
			PointLight* p = (PointLight*)pointLight;
			if (p->mLifeTime > 0)
			{
				p->mLifeTime -= dt;
				if (p->mLifeTime <= 0 && !p->mManualDeletion)
				{
					deleted.push_back(i);
				}
			}
			++i;
		}

		int numDeleted = (int)deleted.size();
		for (int i = numDeleted - 1; i >= 0; i++)
		{
			FB_DELETE(*(mPointLights.begin() + deleted[i]));
			mPointLights.erase(mPointLights.begin() + deleted[i]);			
		}
		deleted.clear();
	}
	
	IPointLight* PointLightMan::CreatePointLight(const Vec3& pos, float range, const Vec3& color, float intensity, float lifeTime, bool manualDeletion)
	{
		mPointLights.push_back(FB_NEW(PointLight)(pos, range, color, intensity, lifeTime, manualDeletion));
		return mPointLights.back();
	}
	void PointLightMan::DeletePointLight(IPointLight* pointLight)
	{
		DeleteValuesInVector(mPointLights, pointLight);
		FB_DELETE(pointLight);
	}

	void PointLightMan::GatherPointLightData(const Vec3& pos, POINT_LIGHT_CONSTANTS* plConst)
	{
		struct GatheredData
		{
			GatheredData(float distSQ, unsigned idx)
			:mDistanceSQ(distSQ), mIndex(idx)
			{

			}
			float mDistanceSQ;
			unsigned mIndex;
		};
		static std::vector<GatheredData> gathered;
		gathered.reserve(50);

		unsigned i = 0;
		for (auto& pointLight : mPointLights)
		{
			PointLight* p = (PointLight*)pointLight;
			float distSQ = p->GetPos().DistanceToSQ(pos);
			if ( distSQ < (p->mRange*p->mRange))
			{
				gathered.push_back(GatheredData(distSQ, i));
			}
			++i;
		}

		std::sort(gathered.begin(), gathered.end(), [](const GatheredData& a, const GatheredData& b){
			return a.mDistanceSQ < b.mDistanceSQ;
		}
		);

		plConst->gPointLightColor[0].w = 0;
		int count = std::min(3, (int)gathered.size());
		for (int i = 0; i < count; i++)
		{
			PointLight* p = (PointLight*)mPointLights[gathered[i].mIndex];
			// pos and range
			plConst->gPointLightPos[i] = Vec4(p->GetPos(), p->GetRange());
			// color and count
			plConst->gPointLightColor[i] = Vec4(p->mColorPowered, (float)count);
			//memcpy(&objConst->gPointLightColor[i].w, &count, 4);
		}

		gathered.clear();
	}

}
