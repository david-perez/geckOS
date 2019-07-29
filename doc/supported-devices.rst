.. _supported_devices:

Supported devices
=================

geckOS can be very easily compiled [#]_ for and run on *any* USB-enabled
Silicon Labs EFM32 *microcontroller*. As of writing, this includes
microcontrollers from the following product families:

- `Giant Gecko S1 <https://www.silabs.com/products/mcu/32-bit/efm32-giant-gecko-s1>`_
- `Happy Gecko    <https://www.silabs.com/products/mcu/32-bit/happy-gecko>`_
- `Leopard Gecko  <https://www.silabs.com/products/mcu/32-bit/leopard-gecko>`_
- `Giant Gecko    <https://www.silabs.com/products/mcu/32-bit/giant-gecko>`_
- `Wonder Gecko   <https://www.silabs.com/products/mcu/32-bit/wonder-gecko>`_

However, geckOS's main benefit is to abstract away device interaction in
*programmable devices* housing one of the above microcontroller units. We have
mainly used geckOS on starter kits or programmable devices based on starter
kits:

- `EFM32 Happy Gecko SLSTK3400A
  <https://www.silabs.com/products/development-tools/mcu/32-bit/efm32-happy-gecko-starter-kit>`_
  (housing a EFM32HG322F64 microcontroller with 64 KiB flash and 8 KiB RAM).
- `STACK device <http://www.cs.ox.ac.uk/people/alex.rogers/stack/>`_, whose
  manufacturing is currently being prototyped with an EFM32 Happy Gecko
  SLSTK3400A.

The :ref:`samples <samples>` section contains applications written for geckOS to be run on
these programmable devices.

.. [#] Largely thanks to the ``efm32-base`` (`link
       <https://github.com/ryankurte/efm32-base>`_) project.
