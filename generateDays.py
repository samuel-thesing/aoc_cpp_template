import os

year = "2025"

class Template:
    name: str = None
    content: str = None

    def __init__(self, path: str):
        self.name = os.path.basename(path)
        with open(path) as file:
            self.content = file.read()

    def get_replaced(self, day: int) -> str:
        return (self.content
                .replace("REPLACE_DAY_NUM", str(day))
                .replace("REPLACE_DAY", f"{day:02}")
                .replace("REPLACE_YEAR", year))



templates = []
for template_name in os.listdir("templates"):
    templates.append(Template(f"templates/{template_name}"))

num_updated = 0
num_created = 0

def create_day(day: int) -> bool:
    global num_created, num_updated

    day_name = f"{day:02}"
    dir_path = f"./src/{day_name}"

    created = False
    if not os.path.isdir(dir_path):
        os.makedirs(dir_path)
        created = True

    updated = False
    for template in templates:
        file_path = f"{dir_path}/{template.name}"
        if os.path.isfile(file_path):
            continue

        updated = True
        with open(file_path, "x") as file:
            file.write(template.get_replaced(day))

    if created:
        num_created += 1
    elif updated:
        num_updated += 1


for day in range(1, 12+1):
    create_day(day)

print(f"Created {num_created} days")
print(f"Updated {num_updated} days")
