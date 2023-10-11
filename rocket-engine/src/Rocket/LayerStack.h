#pragma once

#include "Rocket/Core.h"
#include "Layer.h"

namespace Rocket {
	
	class ROCKET_API LayerStack {

	public:
		LayerStack();
		~LayerStack();

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* overlay);
		void popLayer(Layer* layer);
		void popOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { return m_layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_layers.end(); }

	private:
		std::vector<Layer*> m_layers;
		std::vector<Layer*>::iterator m_layerInsert;
	};

} //namespace Rocket

