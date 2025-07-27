# TreeToMermaid / HwTool

Goals were:

✅ Learning!

✅ Library design.

✅ Learn handling XML files. **For the project i used TinyXML2**

✅ Import HW from CSV and combine to exsisting .hw.  **Works somewhat im very specific cases and with specific cards**

❌ API that is impossible to miss use. **Currenly API is easy to miss use**

❌ Rendering **Bad implementation of the library and time limits didn't allow this**

❌ Tests **Didnt make tests**

TreeToMermaid is a tool for importing, manipulating, and exporting hardware configuration data, with a focus on visualizing module connections as Mermaid diagrams. Project is created with Automation studio in mind. Just barely working and will have alot of unwanted behavior.


### Usage

Importing valid [Hardware.hw](Hardware.hw) and exporting it in mermaid should look like this. Make sure you move hardware.hw or test.csv to the working directory.
```cpp
#include <HwTool.h>
HwTool::Hw hw;
hw.importHW("Hardware.hw")
hw.exportMermaid("Hardware.md")
```


```mermaid
graph TD
    subgraph BM11s ["Base Modules"]
        X20BB52["X20BB52<br/>X20BB52"]
        X20BM11c["X20BM11c<br/>X20BM11"]
        X20BM11b["X20BM11b<br/>X20BM11"]
        X20BM11["X20BM11<br/>X20BM11"]
        X20BM11a["X20BM11a<br/>X20BM11"]
        X20BM11d["X20BM11d<br/>X20BM11"]
    end
    X20TB12d["X20TB12d<br/>X20TB12"]
    X20TB12c["X20TB12c<br/>X20TB12"]
    X20TB12b["X20TB12b<br/>X20TB12"]
    X20TB12a["X20TB12a<br/>X20TB12"]
    AF400["AF400<br/>X20DI9372"]
    X20BB52["X20BB52<br/>X20BB52"]
    X20BM11c["X20BM11c<br/>X20BM11"]
    X20CP0484_1["X20CP0484_1<br/>X20CP0484_1"]
    X20TB12e["X20TB12e<br/>X20TB12"]
    X20BM11b["X20BM11b<br/>X20BM11"]
    AFtest["AFtest<br/>X20AI4632"]
    AF300["AF300<br/>X20DO9322"]
    X20BM11["X20BM11<br/>X20BM11"]
    X20BM11a["X20BM11a<br/>X20BM11"]
    X20TB12["X20TB12<br/>X20TB12"]
    X20AI4632["X20AI4632<br/>X20AI4632"]
    X20BM11d["X20BM11d<br/>X20BM11"]
    X20AI4622["X20AI4622<br/>X20AI4622"]
    X20PS9600["X20PS9600<br/>X20PS9600"]
    AF400 -- SS1 --> X20TB12b
    AF400 -- SL --> X20BM11a
    X20BM11c -- X2X1 --> X20BM11b
    X20CP0484_1 -- SL --> X20BB52
    X20BM11b -- X2X1 --> X20BM11a
    AFtest -- SS1 --> X20TB12e
    AFtest -- SL --> X20BM11d
    AF300 -- SS1 --> X20TB12a
    AF300 -- SL --> X20BM11
    X20BM11 -- X2X1 --> X20BB52
    X20BM11a -- X2X1 --> X20BM11
    X20AI4632 -- SS1 --> X20TB12d
    X20AI4632 -- SL --> X20BM11c
    X20BM11d -- X2X1 --> X20BM11c
    X20AI4622 -- SS1 --> X20TB12c
    X20AI4622 -- SL --> X20BM11b
    X20PS9600 -- PS --> X20BB52
    X20PS9600 -- SS1 --> X20TB12
```

### Working with imported HW


Using getAvailableCards() returns available cards from the currently imported hardware. You must have imported hardware before using getAvailableCards().
```cpp
hw.importHW("hardware.hw");
hw.createCard("testing", cardType::X20AI2622);
std::vector<std::string> validcards = hw.getAvailableCards();
for (size_t i = 0; i < validCards.size(); ++i) {
    std::cout << std::format("[{:>2}] - {}\n", i, validCards[i]);
}
```

```console
[ 0] - AF400
[ 1] - X20CP0484_1
[ 2] - AFtest
[ 3] - AF300
[ 4] - X20AI4632
[ 5] - X20AI4622
```

### Importing from CSV

importCSV returns csv in format that the HwTool can process it.

*HW_version-1.0.0.csv*
```csv
Location,Name,Type
+AK2,AF100,X20DIAI4622
+AK2,AF101,X20DIAI4622
+AK2,AF102,X20DIAI4622
+AK2,AF103,X20DIAI4622
+AK2,AF104,X20DIAI4622
```

```cpp
auto modulesCsv = hw.importCSV("HW_version-1.0.0.csv");

```

### Combining existing hardware.hw with importCSV

Note that the code is buggy and you cannot give target card for comnbineToExisting that isn't the "end of the chain" card. In this [Hardware.hw](Hardware.hw) it is AFtest. So validCards[4] is refering to AFtest.

```cpp
hw.importHW("hardware.hw");
std::vector<std::string> validcards = hw.getAvailableCards();
for (size_t i = 0; i < validCards.size(); ++i) {
    std::cout << std::format("[{:>2}] - {}\n", i, validCards[i]);
}

auto modulesCsv = hw.importCSV("HW_version-1.0.0.csv");
hw.combineToExisting(modulesCsv, validcards[4]);
hw.exportMermaid("HW_version-1.0.0.md");
hw.exportHW("Hw_version1.0.0.0.hw");
```

```mermaid
graph TD
    subgraph BM11s ["Base Modules"]
        X20BM11i["X20BM11i<br/>X20BM11"]
        X20BM11d["X20BM11d<br/>X20BM11"]
        X20BM11f["X20BM11f<br/>X20BM11"]
        X20BM11a["X20BM11a<br/>X20BM11"]
        X20BM11b["X20BM11b<br/>X20BM11"]
        X20BM11["X20BM11<br/>X20BM11"]
        X20BM11c["X20BM11c<br/>X20BM11"]
        X20BB52["X20BB52<br/>X20BB52"]
        X20BM11h["X20BM11h<br/>X20BM11"]
        X20BM11g["X20BM11g<br/>X20BM11"]
        X20BM11e["X20BM11e<br/>X20BM11"]
    end
    X20BM11i["X20BM11i<br/>X20BM11"]
    X20TB12i["X20TB12i<br/>X20TB12"]
    X20PS9600["X20PS9600<br/>X20PS9600"]
    X20AI4622["X20AI4622<br/>X20AI4622"]
    X20BM11d["X20BM11d<br/>X20BM11"]
    X20AI4632["X20AI4632<br/>X20AI4632"]
    X20TB12["X20TB12<br/>X20TB12"]
    X20BM11f["X20BM11f<br/>X20BM11"]
    X20BM11a["X20BM11a<br/>X20BM11"]
    AF300["AF300<br/>X20DO9322"]
    AFtest["AFtest<br/>X20AI4632"]
    X20BM11b["X20BM11b<br/>X20BM11"]
    X20TB12e["X20TB12e<br/>X20TB12"]
    AF103["AF103<br/>X20AI4622"]
    X20BM11["X20BM11<br/>X20BM11"]
    X20CP0484_1["X20CP0484_1<br/>X20CP0484_1"]
    X20TB12h["X20TB12h<br/>X20TB12"]
    X20BM11c["X20BM11c<br/>X20BM11"]
    X20BB52["X20BB52<br/>X20BB52"]
    X20BM11h["X20BM11h<br/>X20BM11"]
    X20BM11g["X20BM11g<br/>X20BM11"]
    AF400["AF400<br/>X20DI9372"]
    X20TB12a["X20TB12a<br/>X20TB12"]
    X20TB12b["X20TB12b<br/>X20TB12"]
    X20TB12c["X20TB12c<br/>X20TB12"]
    X20TB12d["X20TB12d<br/>X20TB12"]
    X20TB12j["X20TB12j<br/>X20TB12"]
    X20TB12g["X20TB12g<br/>X20TB12"]
    X20BM11e["X20BM11e<br/>X20BM11"]
    AF100["AF100<br/>X20AI4622"]
    AF102["AF102<br/>X20AI4622"]
    AF104["AF104<br/>X20AI4622"]
    X20TB12f["X20TB12f<br/>X20TB12"]
    AF101["AF101<br/>X20AI4622"]
    X20BM11i -- X2X1 --> X20BM11h
    X20PS9600 -- PS --> X20BB52
    X20PS9600 -- SS1 --> X20TB12
    X20AI4622 -- SS1 --> X20TB12c
    X20AI4622 -- SL --> X20BM11b
    X20BM11d -- X2X1 --> X20BM11c
    X20AI4632 -- SS1 --> X20TB12d
    X20AI4632 -- SL --> X20BM11c
    X20BM11f -- X2X1 --> X20BM11e
    X20BM11a -- X2X1 --> X20BM11
    AF300 -- SS1 --> X20TB12a
    AF300 -- SL --> X20BM11
    AFtest -- SS1 --> X20TB12e
    AFtest -- SL --> X20BM11d
    X20BM11b -- X2X1 --> X20BM11a
    AF103 -- SS1 --> X20TB12i
    AF103 -- SL --> X20BM11h
    X20BM11 -- X2X1 --> X20BB52
    X20CP0484_1 -- SL --> X20BB52
    X20BM11c -- X2X1 --> X20BM11b
    X20BM11h -- X2X1 --> X20BM11g
    X20BM11g -- X2X1 --> X20BM11f
    AF400 -- SS1 --> X20TB12b
    AF400 -- SL --> X20BM11a
    X20BM11e -- X2X1 --> X20BM11d
    AF100 -- SS1 --> X20TB12f
    AF100 -- SL --> X20BM11e
    AF102 -- SS1 --> X20TB12h
    AF102 -- SL --> X20BM11g
    AF104 -- SS1 --> X20TB12j
    AF104 -- SL --> X20BM11i
    AF101 -- SS1 --> X20TB12g
    AF101 -- SL --> X20BM11f
```

### Adding single cards to HW



```cpp
hw.importHW("hardware.hw");
hw.exportMermaid("adding_single_card_before.md");
hw.createCard("SingleCard", cardType::X20DI9372);
auto validCards = hw.getAvailableCards();
hw.linkToTarget(validCards[2]);
hw.exportMermaid("adding_single_card_after.md");
```
Before adding: 
```mermaid
graph TD
    subgraph BM11s ["Base Modules"]
        X20BB52["X20BB52<br/>X20BB52"]
        X20BM11c["X20BM11c<br/>X20BM11"]
        X20BM11b["X20BM11b<br/>X20BM11"]
        X20BM11["X20BM11<br/>X20BM11"]
        X20BM11a["X20BM11a<br/>X20BM11"]
        X20BM11d["X20BM11d<br/>X20BM11"]
    end
    X20TB12d["X20TB12d<br/>X20TB12"]
    X20TB12c["X20TB12c<br/>X20TB12"]
    X20TB12b["X20TB12b<br/>X20TB12"]
    X20TB12a["X20TB12a<br/>X20TB12"]
    AF400["AF400<br/>X20DI9372"]
    X20BB52["X20BB52<br/>X20BB52"]
    X20BM11c["X20BM11c<br/>X20BM11"]
    X20CP0484_1["X20CP0484_1<br/>X20CP0484_1"]
    X20TB12e["X20TB12e<br/>X20TB12"]
    X20BM11b["X20BM11b<br/>X20BM11"]
    AFtest["AFtest<br/>X20AI4632"]
    AF300["AF300<br/>X20DO9322"]
    X20BM11["X20BM11<br/>X20BM11"]
    X20BM11a["X20BM11a<br/>X20BM11"]
    X20TB12["X20TB12<br/>X20TB12"]
    X20AI4632["X20AI4632<br/>X20AI4632"]
    X20BM11d["X20BM11d<br/>X20BM11"]
    X20AI4622["X20AI4622<br/>X20AI4622"]
    X20PS9600["X20PS9600<br/>X20PS9600"]
    AF400 -- SS1 --> X20TB12b
    AF400 -- SL --> X20BM11a
    X20BM11c -- X2X1 --> X20BM11b
    X20CP0484_1 -- SL --> X20BB52
    X20BM11b -- X2X1 --> X20BM11a
    AFtest -- SS1 --> X20TB12e
    AFtest -- SL --> X20BM11d
    AF300 -- SS1 --> X20TB12a
    AF300 -- SL --> X20BM11
    X20BM11 -- X2X1 --> X20BB52
    X20BM11a -- X2X1 --> X20BM11
    X20AI4632 -- SS1 --> X20TB12d
    X20AI4632 -- SL --> X20BM11c
    X20BM11d -- X2X1 --> X20BM11c
    X20AI4622 -- SS1 --> X20TB12c
    X20AI4622 -- SL --> X20BM11b
    X20PS9600 -- PS --> X20BB52
    X20PS9600 -- SS1 --> X20TB12
```
After adding:

```mermaid
graph TD
    subgraph BM11s ["Base Modules"]
        X20BM11e["X20BM11e<br/>X20BM11"]
        X20BB52["X20BB52<br/>X20BB52"]
        X20BM11c["X20BM11c<br/>X20BM11"]
        X20BM11b["X20BM11b<br/>X20BM11"]
        X20BM11["X20BM11<br/>X20BM11"]
        X20BM11a["X20BM11a<br/>X20BM11"]
        X20BM11d["X20BM11d<br/>X20BM11"]
    end
    SingleCard["SingleCard<br/>X20AI4622"]
    X20TB12f["X20TB12f<br/>X20TB12"]
    X20BM11e["X20BM11e<br/>X20BM11"]
    X20TB12d["X20TB12d<br/>X20TB12"]
    X20TB12c["X20TB12c<br/>X20TB12"]
    X20TB12b["X20TB12b<br/>X20TB12"]
    X20TB12a["X20TB12a<br/>X20TB12"]
    AF400["AF400<br/>X20DI9372"]
    X20BB52["X20BB52<br/>X20BB52"]
    X20BM11c["X20BM11c<br/>X20BM11"]
    X20CP0484_1["X20CP0484_1<br/>X20CP0484_1"]
    X20TB12e["X20TB12e<br/>X20TB12"]
    X20BM11b["X20BM11b<br/>X20BM11"]
    AFtest["AFtest<br/>X20AI4632"]
    AF300["AF300<br/>X20DO9322"]
    X20BM11["X20BM11<br/>X20BM11"]
    X20BM11a["X20BM11a<br/>X20BM11"]
    X20TB12["X20TB12<br/>X20TB12"]
    X20AI4632["X20AI4632<br/>X20AI4632"]
    X20BM11d["X20BM11d<br/>X20BM11"]
    X20AI4622["X20AI4622<br/>X20AI4622"]
    X20PS9600["X20PS9600<br/>X20PS9600"]
    SingleCard -- SS1 --> X20TB12f
    SingleCard -- SL --> X20BM11e
    X20BM11e -- X2X1 --> X20BM11d
    AF400 -- SS1 --> X20TB12b
    AF400 -- SL --> X20BM11a
    X20BM11c -- X2X1 --> X20BM11b
    X20CP0484_1 -- SL --> X20BB52
    X20BM11b -- X2X1 --> X20BM11a
    AFtest -- SS1 --> X20TB12e
    AFtest -- SL --> X20BM11d
    AF300 -- SS1 --> X20TB12a
    AF300 -- SL --> X20BM11
    X20BM11 -- X2X1 --> X20BB52
    X20BM11a -- X2X1 --> X20BM11
    X20AI4632 -- SS1 --> X20TB12d
    X20AI4632 -- SL --> X20BM11c
    X20BM11d -- X2X1 --> X20BM11c
    X20AI4622 -- SS1 --> X20TB12c
    X20AI4622 -- SL --> X20BM11b
    X20PS9600 -- PS --> X20BB52
    X20PS9600 -- SS1 --> X20TB12
```
### Adding multiple cards to HW

```cpp
hw.importHW("hardware.hw");
hw.exportMermaid("before_adding_cards.md");
std::string previousCard = "";
for (size_t i = 0; i < 4; i++)
{
    std::string newCardName = "AF10" + std::to_string(i);
    hw.createCard(newCardName, cardType::X20AI4622);
    if (previousCard.empty()){
        auto validCards2 = hw.getAvailableCards();
        previousCard = validCards2[2];
    }
    
    hw.linkToTarget(previousCard);
    previousCard = newCardName;
}
hw.exportMermaid("after_adding_cards.md");

```

Before adding:

```mermaid
graph TD
    subgraph BM11s ["Base Modules"]
        X20BB52["X20BB52<br/>X20BB52"]
        X20BM11c["X20BM11c<br/>X20BM11"]
        X20BM11b["X20BM11b<br/>X20BM11"]
        X20BM11["X20BM11<br/>X20BM11"]
        X20BM11a["X20BM11a<br/>X20BM11"]
        X20BM11d["X20BM11d<br/>X20BM11"]
    end
    X20TB12d["X20TB12d<br/>X20TB12"]
    X20TB12c["X20TB12c<br/>X20TB12"]
    X20TB12b["X20TB12b<br/>X20TB12"]
    X20TB12a["X20TB12a<br/>X20TB12"]
    AF400["AF400<br/>X20DI9372"]
    X20BB52["X20BB52<br/>X20BB52"]
    X20BM11c["X20BM11c<br/>X20BM11"]
    X20CP0484_1["X20CP0484_1<br/>X20CP0484_1"]
    X20TB12e["X20TB12e<br/>X20TB12"]
    X20BM11b["X20BM11b<br/>X20BM11"]
    AFtest["AFtest<br/>X20AI4632"]
    AF300["AF300<br/>X20DO9322"]
    X20BM11["X20BM11<br/>X20BM11"]
    X20BM11a["X20BM11a<br/>X20BM11"]
    X20TB12["X20TB12<br/>X20TB12"]
    X20AI4632["X20AI4632<br/>X20AI4632"]
    X20BM11d["X20BM11d<br/>X20BM11"]
    X20AI4622["X20AI4622<br/>X20AI4622"]
    X20PS9600["X20PS9600<br/>X20PS9600"]
    AF400 -- SS1 --> X20TB12b
    AF400 -- SL --> X20BM11a
    X20BM11c -- X2X1 --> X20BM11b
    X20CP0484_1 -- SL --> X20BB52
    X20BM11b -- X2X1 --> X20BM11a
    AFtest -- SS1 --> X20TB12e
    AFtest -- SL --> X20BM11d
    AF300 -- SS1 --> X20TB12a
    AF300 -- SL --> X20BM11
    X20BM11 -- X2X1 --> X20BB52
    X20BM11a -- X2X1 --> X20BM11
    X20AI4632 -- SS1 --> X20TB12d
    X20AI4632 -- SL --> X20BM11c
    X20BM11d -- X2X1 --> X20BM11c
    X20AI4622 -- SS1 --> X20TB12c
    X20AI4622 -- SL --> X20BM11b
    X20PS9600 -- PS --> X20BB52
    X20PS9600 -- SS1 --> X20TB12
```

After adding:
```mermaid
graph TD
    subgraph BM11s ["Base Modules"]
        X20BM11d["X20BM11d<br/>X20BM11"]
        X20BM11b["X20BM11b<br/>X20BM11"]
        X20BM11["X20BM11<br/>X20BM11"]
        X20BM11c["X20BM11c<br/>X20BM11"]
        X20BB52["X20BB52<br/>X20BB52"]
        X20BM11a["X20BM11a<br/>X20BM11"]
        X20BM11f["X20BM11f<br/>X20BM11"]
        X20BM11e["X20BM11e<br/>X20BM11"]
        X20BM11g["X20BM11g<br/>X20BM11"]
        X20BM11h["X20BM11h<br/>X20BM11"]
    end
    AF103["AF103<br/>X20AI4622"]
    X20TB12i["X20TB12i<br/>X20TB12"]
    X20PS9600["X20PS9600<br/>X20PS9600"]
    X20AI4622["X20AI4622<br/>X20AI4622"]
    X20BM11d["X20BM11d<br/>X20BM11"]
    X20AI4632["X20AI4632<br/>X20AI4632"]
    X20TB12["X20TB12<br/>X20TB12"]
    AF300["AF300<br/>X20DO9322"]
    AFtest["AFtest<br/>X20AI4632"]
    X20BM11b["X20BM11b<br/>X20BM11"]
    X20TB12e["X20TB12e<br/>X20TB12"]
    X20BM11["X20BM11<br/>X20BM11"]
    X20CP0484_1["X20CP0484_1<br/>X20CP0484_1"]
    X20TB12h["X20TB12h<br/>X20TB12"]
    X20BM11c["X20BM11c<br/>X20BM11"]
    X20BB52["X20BB52<br/>X20BB52"]
    AF400["AF400<br/>X20DI9372"]
    X20TB12a["X20TB12a<br/>X20TB12"]
    X20TB12b["X20TB12b<br/>X20TB12"]
    X20TB12c["X20TB12c<br/>X20TB12"]
    X20BM11a["X20BM11a<br/>X20BM11"]
    X20BM11f["X20BM11f<br/>X20BM11"]
    X20TB12d["X20TB12d<br/>X20TB12"]
    X20TB12g["X20TB12g<br/>X20TB12"]
    X20BM11e["X20BM11e<br/>X20BM11"]
    X20TB12f["X20TB12f<br/>X20TB12"]
    AF101["AF101<br/>X20AI4622"]
    AF100["AF100<br/>X20AI4622"]
    AF102["AF102<br/>X20AI4622"]
    X20BM11g["X20BM11g<br/>X20BM11"]
    X20BM11h["X20BM11h<br/>X20BM11"]
    AF103 -- SS1 --> X20TB12i
    AF103 -- SL --> X20BM11h
    X20PS9600 -- PS --> X20BB52
    X20PS9600 -- SS1 --> X20TB12
    X20AI4622 -- SS1 --> X20TB12c
    X20AI4622 -- SL --> X20BM11b
    X20BM11d -- X2X1 --> X20BM11c
    X20AI4632 -- SS1 --> X20TB12d
    X20AI4632 -- SL --> X20BM11c
    AF300 -- SS1 --> X20TB12a
    AF300 -- SL --> X20BM11
    AFtest -- SS1 --> X20TB12e
    AFtest -- SL --> X20BM11d
    X20BM11b -- X2X1 --> X20BM11a
    X20BM11 -- X2X1 --> X20BB52
    X20CP0484_1 -- SL --> X20BB52
    X20BM11c -- X2X1 --> X20BM11b
    AF400 -- SS1 --> X20TB12b
    AF400 -- SL --> X20BM11a
    X20BM11a -- X2X1 --> X20BM11
    X20BM11f -- X2X1 --> X20BM11e
    X20BM11e -- X2X1 --> X20BM11d
    AF101 -- SS1 --> X20TB12g
    AF101 -- SL --> X20BM11f
    AF100 -- SS1 --> X20TB12f
    AF100 -- SL --> X20BM11e
    AF102 -- SS1 --> X20TB12h
    AF102 -- SL --> X20BM11g
    X20BM11g -- X2X1 --> X20BM11f
    X20BM11h -- X2X1 --> X20BM11g
```

### Undo Redo

Going of the previous example. Undoing 3 times will leave just one newly added card "AF100". Mermaid charts sometimes changes the display format. Redo works the same way but to the other direction. Redo and undo is only implemented for createCard currently.

```cpp
    ...
    hw.undo();
    hw.undo();
    hw.undo();

    hw.exportMermaid("after_undo_cards.md");
```

```mermaid
graph TD
    subgraph BM11s ["Base Modules"]
        X20BM11d["X20BM11d<br/>X20BM11"]
        X20BM11b["X20BM11b<br/>X20BM11"]
        X20BM11["X20BM11<br/>X20BM11"]
        X20BM11c["X20BM11c<br/>X20BM11"]
        X20BB52["X20BB52<br/>X20BB52"]
        X20BM11a["X20BM11a<br/>X20BM11"]
        X20BM11e["X20BM11e<br/>X20BM11"]
    end
    X20PS9600["X20PS9600<br/>X20PS9600"]
    X20AI4622["X20AI4622<br/>X20AI4622"]
    X20BM11d["X20BM11d<br/>X20BM11"]
    X20AI4632["X20AI4632<br/>X20AI4632"]
    X20TB12["X20TB12<br/>X20TB12"]
    AF300["AF300<br/>X20DO9322"]
    AFtest["AFtest<br/>X20AI4632"]
    X20BM11b["X20BM11b<br/>X20BM11"]
    X20TB12e["X20TB12e<br/>X20TB12"]
    X20BM11["X20BM11<br/>X20BM11"]
    X20CP0484_1["X20CP0484_1<br/>X20CP0484_1"]
    X20BM11c["X20BM11c<br/>X20BM11"]
    X20BB52["X20BB52<br/>X20BB52"]
    AF400["AF400<br/>X20DI9372"]
    X20TB12a["X20TB12a<br/>X20TB12"]
    X20TB12b["X20TB12b<br/>X20TB12"]
    X20TB12c["X20TB12c<br/>X20TB12"]
    X20BM11a["X20BM11a<br/>X20BM11"]
    X20TB12d["X20TB12d<br/>X20TB12"]
    X20BM11e["X20BM11e<br/>X20BM11"]
    X20TB12f["X20TB12f<br/>X20TB12"]
    AF100["AF100<br/>X20AI4622"]
    X20PS9600 -- PS --> X20BB52
    X20PS9600 -- SS1 --> X20TB12
    X20AI4622 -- SS1 --> X20TB12c
    X20AI4622 -- SL --> X20BM11b
    X20BM11d -- X2X1 --> X20BM11c
    X20AI4632 -- SS1 --> X20TB12d
    X20AI4632 -- SL --> X20BM11c
    AF300 -- SS1 --> X20TB12a
    AF300 -- SL --> X20BM11
    AFtest -- SS1 --> X20TB12e
    AFtest -- SL --> X20BM11d
    X20BM11b -- X2X1 --> X20BM11a
    X20BM11 -- X2X1 --> X20BB52
    X20CP0484_1 -- SL --> X20BB52
    X20BM11c -- X2X1 --> X20BM11b
    AF400 -- SS1 --> X20TB12b
    AF400 -- SL --> X20BM11a
    X20BM11a -- X2X1 --> X20BM11e
    X20BM11e -- X2X1 --> X20BM11
    AF100 -- SS1 --> X20TB12f
    AF100 -- SL --> X20BM11e
```

### Exporting HW

After adding cards you can export it back to HW. If the MermaidGraph looks allright it should be valid .hw.

```cpp
auto modulesCsv = hw.importCSV("HW_version-1.0.0.csv");
hw.combineToExisting(modulesCsv, validcards5[2]);
hw.exportMermaid("HW_version-1.0.0.md");
hw.exportHW("Hw_version1.0.0.0.hw");
printf("Example done");
```

### LISENCE
MIT