import pytest
import os

qemu_images = []
qemu_images_path = 'qemu_test/qemu_test_images'

# Use os.walk to traverse the directory and its subdirectories
for root, dirs, files in os.walk(qemu_images_path):
    for file in files:
        if file.endswith('.img'):
            file_path = os.path.join(root, file)
            qemu_images.append(file_path)

# Print the list of image filenames
print('The list entries are ------------')
for image in qemu_images:
    print(image)


@pytest.mark.parametrize('qemu_image_path', qemu_images, indirect=True)
@pytest.mark.parametrize('skip_regenerate_image', ['y'], indirect=True)
def test_esp_secure_cert(dut):
    dut.expect('Successfully obtained and verified the '
               'contents of esp_secure_cert partition')
