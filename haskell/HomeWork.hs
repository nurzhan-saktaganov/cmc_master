module HomeWork (compressText, decompressText) where

import Data.Map
import Data.List
import Data.Tree
import Data.Maybe

import Numeric (showIntAtBase)
import Data.Char (intToDigit)

--newtype Tup a b = Tup (a, b)

--instance Show(Tup Char Int) where
--    show (Tup(char, int)) = "(" ++ show char ++ "," ++ show int ++ ")"

elemsFreq :: String -> [(Char, Int)]
elemsFreq string = toList $ fromListWith (+) [(c, 1) | c <- string]

elemsFreqToTreeList :: [(Char, Int)] -> Forest (Char, Int)
elemsFreqToTreeList freqList = [ Node x [] | x <- freqList ]

sortForest :: Forest (Char, Int) -> Forest (Char, Int)
sortForest x = sortBy (\t1 t2 -> compare (snd $ getTreeRootElem t1) (snd $ getTreeRootElem t2)) x

getTreeRootElem :: Tree (Char, Int) -> (Char, Int)
getTreeRootElem tree = head $ head $ levels tree

--sortFreqList :: [(Char, Int)] -> [(Char, Int)]
--sortFreqList x = sortBy (\p1 p2 -> compare (snd p1) (snd p2)) x

buildHuffmanTree :: Forest (Char, Int) -> Tree (Char, Int)
buildHuffmanTree sortedForest
    | length sortedForest == 1 = head sortedForest
    | otherwise = let
                    (first:tail) = sortedForest
                    (second:dummy) = tail
                    char = '\0'
                    pair1 = getTreeRootElem first
                    pair2 = getTreeRootElem second
                    int = snd (pair1) + snd (pair2)
                    merged = Node (char, int) [first, second]
                  in buildHuffmanTree $ sortForest (merged:dummy)

buildHuffmanTreeFor :: String -> Tree (Char, Int)
buildHuffmanTreeFor string =  buildHuffmanTree $ sortForest $ elemsFreqToTreeList $ elemsFreq string

buildHuffmanTableFor :: String -> [(Char, String)]
buildHuffmanTableFor string = buildTableFrom $ buildHuffmanTreeFor string

isLeaf :: Tree (Char, Int) -> Bool
isLeaf tree = (==) 1 $ length $ flatten $ tree

buildTableFrom :: Tree (Char, Int) -> [(Char, String)]
buildTableFrom tree = buildTableFrom' tree ""

buildTableFrom' :: Tree (Char, Int) -> String -> [(Char, String)]
buildTableFrom' tree path
    | isLeaf tree = let
                      char = fst $ getTreeRootElem tree
                    in [(char, path)]
    | otherwise = let
                    (child1:ohter) = subForest tree
                    (child2:dummy) = ohter
                  in (buildTableFrom' child1 (path ++ "0")) ++
                        (buildTableFrom' child2 (path ++ "1"))

getCharCodeFromTable :: [(Char, String)] -> Char -> String
getCharCodeFromTable table char = 
    snd $ fromJust $ find (\p -> fst p == char) table

stringToTableCodes :: [(Char, String)] -> String -> [String]
stringToTableCodes table string =
    --Data.List.map (getCharCodeFromTable table) string
    [getCharCodeFromTable table char | char <- string]

getCharFromCode :: [(Char, String)] -> String -> Char
getCharFromCode table string =
    fst $ fromJust $ find (\p -> snd p == string) table

tableCodesToString :: [(Char, String)] -> [String] -> String
tableCodesToString table strings =
    --Data.List.map (getCharFromCode table) strings
    [getCharFromCode table string | string <- strings]

toBinary :: [String] -> (Integer, Int)
toBinary strings = 
        let
            accHelper = (\acc x -> if x == '0' then (acc * 2, 1) else (acc * 2 + 1, 1))
            accResult = mapAccumL accHelper 0 (concat strings)
            binary = fst accResult
            strLength = sum $ snd accResult
        in (binary, strLength)

fromBinary :: [(Char, String)] -> (Integer, Int) -> [String]
fromBinary table (binary, strLength) =
        let
            concatedStr = fromBinary' binary strLength
            prefixList = [snd pair | pair <- table]
        in  splitIntoPrefixes prefixList [] concatedStr
    where
        fromBinary' binary strLength =
                let
                    strTail = showIntAtBase 2 intToDigit binary ""
                    lenDiff = strLength - length strTail
                    strHead = ['0' | i <- [1..lenDiff]]
                in strHead ++ strTail
        splitIntoPrefixes prefixList result target
                | target == [] = result
                | otherwise =
                        let
                            found = head [prefix | prefix <- prefixList, isPrefixOf prefix target]
                            dropCount = length found
                            target' = drop dropCount target
                            result' = result ++ [found]
                        in splitIntoPrefixes prefixList result' target'

compressText :: String -> ([(Char, String)], (Integer, Int))
compressText string =
        let
            table = buildHuffmanTableFor string
            codes = stringToTableCodes table string
            binary = toBinary codes
        in (table, binary)

decompressText :: ([(Char, String)], (Integer, Int)) -> String
decompressText (table, binary) =
        let
            tableCodes = fromBinary table binary
            string = tableCodesToString table tableCodes
        in string

-- Test case
--getList x n = [x| c<-[1..n]]
--demoString = getList 'A' 15 ++ getList 'B' 7 ++ getList 'V' 6 ++ getList 'G' 6 ++ getList 'D' 5
--demoTable =  buildHuffmanTableFor demoString
--demoCodes = stringToTableCodes demoTable demoString